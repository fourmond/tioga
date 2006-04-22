/* builder.c */

#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h> 
#include <math.h>
#include <string.h>
#include <unistd.h>

   void Build_Models(void) {
      char *fname = "datalog.data"; /* this is the communication file between the builder and the viewer */
      FILE *file;
      const int num_pts = 100;
      int plot_num = 0, i;
      double Xs[num_pts], Y1s[num_pts], Y2s[num_pts], Y3s[num_pts];
      double pi = 2*asin(1.0), amplitude;
      for (i=0; i < num_pts; i++) Xs[i] = (2*pi*i)/(num_pts-1);
      while (true) {
         plot_num++;
         sleep(1); /* this is where a real application would do something interesting */
         for (i=0; i < num_pts; i++) {
            amplitude = exp(-(Xs[i]-Xs[0])*(Xs[i]-Xs[0])/30.0);
            Y1s[i] = cos(sqrt(plot_num*.95+Xs[i])+Xs[i]*Xs[i])*amplitude;
            Y2s[i] = sin(sqrt(plot_num*.65+Xs[i])+Xs[i])*amplitude;
            Y3s[i] = log10(1e-6+Y1s[i]*Y1s[i]);
            }
         /* now that we've calculated the next model, we'll write it to the file so the viewer can show it */
         if ((file=fopen(fname, "w"))==NULL) continue;
         fprintf(file, "%i %i\n", plot_num, num_pts);
         for (i=0; i < num_pts; i++)
            fprintf(file, "%lg %lg %lg %lg\n", Xs[i], Y1s[i], Y2s[i], Y3s[i]);
         fclose(file);
         /* let the user know we're still working */
         if (plot_num == 1 || plot_num % 10 == 0) printf("Finished Model Number %i\n", plot_num);
         }
      }

   int main(int argc, char *argv[]) {
      argc = 0; argv = NULL; /* unused params */
      Build_Models();
      return 0;
      }
