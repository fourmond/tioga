/* Copyright (C) 1991,1992,1996,1997,1999,2004 Free Software Foundation, Inc.
   Copyright (C) 2006 Vincent Fourmond.
   This file is taken from the GNU C Library.
   Written by Douglas C. Schmidt (schmidt@ics.uci.edu), modified by
   Vincent Fourmond to specialize for joint sort of double arrays.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* If you consider tuning this algorithm, you should consult first:
   Engineering a sort function; Jon Bentley and M. Douglas McIlroy;
   Software - Practice and Experience; Vol. 23 (11), 1249-1265, 1993.  */

#include <namespace.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* SWAP has to be completely redefined to take care of swapping *both*
   arrays */

inline static void swap_one(double * a, double * b) 
{
  double tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
}

#define SWAP(a,b) \
do {swap_one(a,b);\
swap_one((a - x_values + y_values), (b - x_values + y_values));}\
while(0)




/* Discontinue quicksort algorithm when partition gets below this size.
   This particular magic number was chosen to work best on a Sun 4/260. */
#define MAX_THRESH 4

/* Stack node declarations used to store unfulfilled partition obligations. */
typedef struct
  {
    double *lo;
    double *hi;
  } stack_node;

/* The next 4 #defines implement a very fast in-line stack abstraction. */
/* The stack needs log (total_elements) entries (we could even subtract
   log(MAX_THRESH)).  Since total_elements has type size_t, we get as
   upper bound for log (total_elements):
   bits per byte (CHAR_BIT) * sizeof(size_t).  */
#define STACK_SIZE	(CHAR_BIT * sizeof(size_t))
#define PUSH(low, high)	((void) ((top->lo = (low)), (top->hi = (high)), ++top))
#define	POP(low, high)	((void) (--top, (low = top->lo), (high = top->hi)))
#define	STACK_NOT_EMPTY	(stack < top)


/* Order size using quicksort.  This implementation incorporates
   four optimizations discussed in Sedgewick:

   1. Non-recursive, using an explicit stack of pointer that store the
      next array partition to sort.  To save time, this maximum amount
      of space required to store an array of SIZE_MAX is allocated on the
      stack.  Assuming a 32-bit (64 bit) integer for size_t, this needs
      only 32 * sizeof(stack_node) == 256 bytes (for 64 bit: 1024 bytes).
      Pretty cheap, actually.

   2. Chose the pivot element using a median-of-three decision tree.
      This reduces the probability of selecting a bad pivot value and
      eliminates certain extraneous comparisons.

   3. Only quicksorts TOTAL_ELEMS / MAX_THRESH partitions, leaving
      insertion sort to order the MAX_THRESH items within each partition.
      This is a big win, since insertion sort is faster for small, mostly
      sorted array segments.

   4. The larger of the two sub-partitions is always pushed onto the
      stack first, with the algorithm then concentrating on the
      smaller partition.  This *guarantees* no more than log (total_elems)
      stack size is needed (actually O(1) in this case)!  */

INTERN void
joint_quicksort (double *const x_values, double * const y_values,
		 size_t total_elems)
{
  double * const base_ptr = x_values;
  if (total_elems == 0)
    /* Avoid lossage with unsigned arithmetic below.  */
    return;

  if (total_elems > MAX_THRESH)
    {
      double *lo = base_ptr;
      double *hi = lo + (total_elems - 1);
      stack_node stack[STACK_SIZE];
      stack_node *top = stack;

      PUSH (NULL, NULL);

      while (STACK_NOT_EMPTY)
        {
          double *left_ptr;
          double *right_ptr;

	  /* Select median value from among LO, MID, and HI. Rearrange
	     LO and HI so the three values are sorted. This lowers the
	     probability of picking a pathological pivot value and
	     skips a comparison for both the LEFT_PTR and RIGHT_PTR in
	     the while loops. */
	  
	  double *mid = lo + ((hi - lo) >> 1);
	  
	  if (*mid < *lo)
	    SWAP (mid, lo);
	  if (*hi < *mid)
	    SWAP (mid, hi);
	  else
	    goto jump_over;
	  if (*mid < *lo)
	    SWAP (mid, lo);
	  
	jump_over:;
	  
	  left_ptr  = lo + 1;
	  right_ptr = hi - 1;

	  /* Here's the famous ``collapse the walls'' section of quicksort.
	     Gotta like those tight inner loops!  They are the main reason
	     that this algorithm runs much faster than others. */
	  do
	    {
	      while (*left_ptr < *mid)
		left_ptr ++;
	      while (*mid < *right_ptr)
		right_ptr --;

	      if (left_ptr < right_ptr)
		{
		  SWAP (left_ptr, right_ptr);
		  if (mid == left_ptr)
		    mid = right_ptr;
		  else if (mid == right_ptr)
		    mid = left_ptr;
		  left_ptr ++;
		  right_ptr --;
		}
	      else if (left_ptr == right_ptr)
		{
		  left_ptr ++;
		  right_ptr --;
		  break;
		}
	    }
	  while (left_ptr <= right_ptr);

          /* Set up pointers for next iteration.  First determine whether
             left and right partitions are below the threshold size.  If so,
             ignore one or both.  Otherwise, push the larger partition's
             bounds on the stack and continue sorting the smaller one. */

          if ((size_t) (right_ptr - lo) <= MAX_THRESH)
            {
              if ((size_t) (hi - left_ptr) <= MAX_THRESH)
		/* Ignore both small partitions. */
                POP (lo, hi);
              else
		/* Ignore small left partition. */
                lo = left_ptr;
            }
          else if ((size_t) (hi - left_ptr) <= MAX_THRESH)
	    /* Ignore small right partition. */
            hi = right_ptr;
          else if ((right_ptr - lo) > (hi - left_ptr))
            {
	      /* Push larger left partition indices. */
              PUSH (lo, right_ptr);
              lo = left_ptr;
            }
          else
            {
	      /* Push larger right partition indices. */
              PUSH (left_ptr, hi);
              hi = right_ptr;
            }
        }
    }

  /* Once the BASE_PTR array is partially sorted by quicksort the rest
     is completely sorted using insertion sort, since this is efficient
     for partitions below MAX_THRESH size. BASE_PTR points to the beginning
     of the array to sort, and END_PTR points at the very last element in
     the array (*not* one beyond it!). */

#define min(x, y) ((x) < (y) ? (x) : (y))

  {
    double *const end_ptr = base_ptr + (total_elems - 1);
    double *tmp_ptr = base_ptr;
    double *thresh = min(end_ptr, base_ptr + MAX_THRESH);
    double *run_ptr;

    /* Find smallest element in first threshold and place it at the
       array's beginning.  This is the smallest array element,
       and the operation speeds up insertion sort's inner loop. */

    for (run_ptr = tmp_ptr + 1; run_ptr <= thresh; run_ptr ++)
      if (*run_ptr < *tmp_ptr)
        tmp_ptr = run_ptr;
    
    if (tmp_ptr != base_ptr)
      SWAP (tmp_ptr, base_ptr);

    /* Insertion sort, running from left-hand-side up to right-hand-side.  */

    run_ptr = base_ptr + 1;
    while ((run_ptr ++) < end_ptr)
      {
	tmp_ptr = run_ptr - 1;
	while (*run_ptr < *tmp_ptr)
	  tmp_ptr --;

	tmp_ptr ++;
        if (tmp_ptr != run_ptr)
          {
	    double tmp = *run_ptr;
	    double tmp_y = *(run_ptr - x_values + y_values);
	    double * trav = run_ptr;
	    while(--trav >= tmp_ptr) 
	      {
		*(trav + 1) = *trav;
		*(trav + 1 - x_values + y_values) = 
		  *(trav - x_values + y_values);
	      }
	    *tmp_ptr = tmp;
	    *(tmp_ptr - x_values + y_values) = tmp_y;
          }
      }

  }
}

/* Hey, it miraculously looks like I got it right... */
