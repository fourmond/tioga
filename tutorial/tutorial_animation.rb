
module Tioga
module Tutorial

=begin rdoc

= How to create a sequence of pdfs and convert it to a movie

If is often useful to create a series of plots showing the changes that occur as some
parameter changes.  For example, in my stellar evolution work, the parameter is time
and the plots are things like the profiles of element abundances from the center of the star
to the surface.  The stellar evolution simulation cranks out a series of models for
increasing ages, and for each model, or each 10th model perhaps, I may want to
save a plot showing the profiles at that time.  In addition to providing a record
that I can consult later, the series of pdfs can also be converted to a movie showing
the evolution.

This section of the documentation discusses doing this in tioga and outlines
the method I use on the Mac for converting the series of pdfs into a movie.

As a concrete example, let's how I make a stellar evolution movie.  First I run
the simulation and save a series of data files for various models created
during the evolution.  These files are in a directory called 'run/models' and
have names like model_767.log that include the model sequence number.  The frames
of the movie will be in the same order as the sequence numbers of the models.

In a different directory I have a file called 'profile_plots.rb' that reads
data in the format of the model files and creates tioga plots of various things.
I want to make a movie using one of those plots for each of the models I've saved.
The following small file takes care of creating the pdfs for the movie.

      # movie_batch.rb

      require 'Tioga/FigureMaker'
      require 'profile_plots.rb'

      class ProfilePlots

          def run(movie_save_dir, file_prefix, file_suffix, 
                  plot_number = 0, first_model = 1, last_model = 2000)
              t.save_dir = movie_save_dir
              n = first_model
              while n <= last_model
                  fname = file_prefix + n.to_s + file_suffix
                  begin 
                      # check to see if the model file exists
                      f = File.open(fname, 'r')
                      f.close
                      # tell ProfilePlots to reload the data
                      @have_data = false
                      puts "\n" + fname
                      t.make_pdf(plot_number)
                      pdf_name = t.figure_pdf(plot_number)
                      name = pdf_name[0..-5] # remove the '.pdf'
                      name = append_sequence_number_to_name(name, n)
                      syscmd = 'mv ' + pdf_name + ' ' + name + '.pdf'
                      system(syscmd)
                  rescue
                      # end up here if the File.open failed
                      # just continue to check the next possible model number
                  end
                  n = n + 1
              end
              puts "\n"
          end
          
          def append_sequence_number_to_name(name, n)
            # always use at least 4 digits.  add leading 0's if necessary.
            name += '_'
            if n < 10
              name += '000'
            elsif n < 100
              name += '00'
            elsif n < 1000
              name += '0'
            end
            name += n.to_s
          end
 
      end

      ProfilePlots.new('../run/EZ_status.log').run('movie_out', '../run/models/model_', '.log')

This file adds a 'run' method to my ProfilePlots class that goes through the run/models
directory and creates a pdf file for each model.  The first argument to run, 'movie_save_dir',
says where the movie pdfs will go.  The arguments 'file_prefix' and 'file_suffix'
get wrapped around each model number to generate a name for a model data file.  
The 'plot_number' argument says which plot to do from ProfilePlots.  The last two arguments,
'first_model' and 'last_model', give the option of selecting a section of models for the movie.
  
Once we've got movie_batch.rb set up, the movie pdfs can be created simply by having ruby run it.

The next job is to convert all those pdfs into a movie.  I'll describe how I do it on a Mac; for linux
one good choice is to use the convert tool from ImageMagick and mencoder from mplayer.

On my Mac, I'm currently using GraphicConverter, Version 5.9.
(If you don't have it, or only have an earlier version, cough up the $30 or so and get it.  The site
is lemkesoft[http://www.lemkesoft.com/en/graphcon.htm].)
GraphicConverter has an operation that does just what we need.  It's called "Export Slide Show to Movie...", and it's found
under the "File" menu.

Before you create the movie, you can adjust the frame size by setting the Preferences for import resolution for 
pdfs.  Open the GraphicConverter Preferences and go to the panel for PDF under Open - Formats.  Make sure it is set to "Open with bit depth of 32 bit".  Change the "Open with resolution of 72 ppi" to "150 ppi".
The bigger the resolution number, the larger the movie frame.  For example, instead of 150, use 100 if you want a smaller movie frame.

Close the Preferences, open up the panel for the "Export Slide Show to Movie..." command (under File), and navigate to the "movie_out" folder.
Hit the "Choose" button and pick a name for the movie and a folder to hold it.  Click "Save".  When
the "Additional Movie Settings" panel appears, check the button that says "Use dimension of first image".
You might also want to adjust the "Delay between frames/images" -- I've used 0.1 seconds in the example.
Select the "Compression..." button to set the parameters for the MPG output.  Set "MPEG-4 Video" for compression type,
uncheck the "Limit data rate" choice, and set "Quality" to "Best".  Click "OK" for "Compression Settings", and then click "OK" again for "Additional Movie Settings".  

That should do it.  
Open your new movie and enjoy.

One final hint regarding movies: you should make every frame a "key frame" if you want to be able to 
manually move back and forth to arbitrary spots in the movie.  Only key frames have a complete image -- non-key 
frames are represented by changes from the previous key frame.  For "normal" movies, it can save a 
lot of storage by having lots of non-key frames, but for our use, it is much better to have all the 
frames be complete.

---

Now let's look at a few "real life" examples using tioga to create complex plots -- next stop: OtherStuff.

=end

module Animation 
end # module Animation

end # module Tutorial
end # module Tioga

