require 'rbconfig'
require 'fileutils'

# some global variables: 
$LOCAL_LIBS = ""

# A string extension from old mkmf.rb
class String
  def quote
    /\s/ =~ self ? "\"#{self}\"" : self
  end

  # and a new function, that can come in useful in several places:
  def sanitize
    return self.tr("a-z./\055", "A-Z___")
  end
end

# An array extension from old mkmf.rb
class Array
  def quote
    map {|s| s.quote}
  end
end


=begin rdoc
:title: Mkmf2: a better replacement for mkmf
=end 

=begin rdoc

=Mkmf2: a drop-in replacement for mkmf

Mkmf2 aims at replacing the functionnality of the old mkmf, with a better and
cleaner code. It hopefully will keep all the previous mkmf files working,
while providing 
* better control over the Makefiles produced;
* installation to <tt>$HOME</tt>;
* uninstallation;
* more flexibility.

==Background

Mkmf2 was written to supplement a missing feature on the old +mkmf.rb+: it is
not possible to install include files along with the library, and not easy
to make several libraries, that link each other, and install them... So here
comes mkmf2.rb !


==Bugs

There are probably a huge number of bugs, or missing features, or
misleading features. In case you find one, please file a request
in the mkmf2 project on rubyforge, at the address
http://rubyforge.org/tracker/?group_id=1391

But before, please, check out the latest cvs version at
http://rubyforge.org/scm/?group_id=1391
and make sure the bug is still there...

==Unimplemented features

* It is currently impossible to remove directories with make uninstall.
  This is due to the fact that +rbconfig.rb+ doesn't provide an entry
  for removing directories.

==Note to anyone who wishes to contribute

Please, document any code you add, and keep the
changelog up to date...

==Copyright

This module is copyright 2006 by Vincent Fourmond. You can use and
redistribute it under the terms of the General Public License version 2.
As a special exception, you can distribute any verbatim copy along with
a program, provided that this program depends on mkmf2 to install.

=end

module Mkmf2

  include Config

  # The CVS tag used for the release.
  CVS_TAG = '$Name$'

  # The module version; it is computed from CVS_TAG
  VERSION = CVS_TAG.match(/\D+(.*?)\s*\$?$/)[1].tr('_-','..')

  # The entities: a list of Mkmf2::MfEntity representing what
  # we are currently building.
  @@entities = []

  # User-defined rules:
  @@user_rules = []


  # we first start the module with a whole bunch of small subclasses
  # that will come in really useful later.

  # This class provides a crude functionnality for defining a rule
  # in a Makefile.
  class MfRule
    attr_accessor :name, :rule, :dependencies

    # converts this rule to a string that can be readily printed to
    # the Makefile
    def to_s
      return "#{@name}: #{@dependencies.join ' '}" + 
        if @rule.empty? # we don't need a second line
          "\n" 
        else
          "\n\t#{@rule.join "\n\t"}\n" 
        end 
    end
    
    # +name+:: the name of the rule.
    # +rule+::
    #   a string or an array of lines; strings are split if necessary
    # +deps+:: a string or an array of dependencies
    def initialize(name, rule = nil, deps = nil)
      @name = name 

      if rule.is_a? Array
        @rule = rule
      elsif rule.nil?
        @rule = []
      else
        @rule = rule.split /\s*\n\t?\s*/
      end

      if deps.is_a? Array
        @dependencies = deps
      elsif deps.nil?
        @dependencies = []
      else
        @dependencies = deps.split /\s+/
      end
    end
    
  end 

  # A small class to include comments into the Makefile, just to help anyone
  # who has to debug something. And it annoys me not to be able to clearly
  # read the output of the test.rb script !
  class MfComment < MfRule
    
    # The actual text of the comment.
    attr_accessor :text

    # +str+ is the text meant to be displayed in the makefile. If it is nil
    # the rule just ends up being a blank line, to allow for easy separation
    # of the rules.
    def initialize(str = nil)
      @text = str
    end

    def to_s
      if @text.nil?
        return "\n"
      else
        lines = @text.to_s.split '\n'
        lines.unshift ""
        return "#{lines.join "\n# "}\n" 
      end
    end
    
  end
    


  # An MfEntity object reprensents something that is part of the package.
  # In this way, it needs to provide:
  # 
  # * a way to *install* itself;
  # * a way to *uninstall* itself;
  # * a way to *clean* the source directory;
  # * and, possibly, a way to *build* itself.
  #
  # This class should be subclassed for objects that need building.
  # It should be enough though for many simple objects.
  #
  # The module will hold a list of all the MfEntities, and use them in turn
  # to create the Makefile. 
  class MfEntity

    # +install_files+ is a hash that associates files in the building
    # tree to files where the installation is done. To be more precise
    # the install path for the files is expressed relative to
    # +install_path+
    attr_accessor :install_files

    # This name has to be registered by Mkmf2.register_name. Its lowercase
    # version will be used to name the targets (install_#{name}), etc.), and
    # it's uppercase version can be used as a prefix to name Makefile
    # variables, that we can for instance depend on.
    attr_accessor :name

    # It represents the type of the entity. It should be a valid entry
    # for Mkmf2.install_path.
    attr_accessor :kind

    # It is used to ask Mkmf2.install_files for the right rule for
    # installing files (they can be data, scripts, programs, and so on...)
    attr_accessor :install_rule

    # The +name+ parameter is turned into a unique name and ends up as
    # an identifier for the stuffs related to this element in the Makefile.
    # The +install+ parameter directly becomes install_files.
    # The +kind+ parameter describes the kind of thing we're going
    # to install. See Mkmf2.install_path.

    def initialize(name, install, kind = 'lib',
                   install_rule = 'install_data')

      # we append kind to avoid overlaps in case of
      # a malchosen name (even if this is guaranteed by register_name),
      # it will look better.
      @name = Mkmf2::register_name(kind + "_" + name)
      @install_files = install.dup
      @kind = kind
      @install_rule = install_rule
    end

    # Returns the install path for the current object. The implementation
    # should call Mkmf2.install_path with an appropriate argument.
    def install_path
      return Mkmf2::install_path(@kind)
    end

    
    # Returns an array containing
    # * a string as the first element which should be appendended
    #   as a dependency for the install target
    # * a set of MfRules, describing what need to be done
    #   for that peculiar install.

    def install_rules
      rules = ["install_#{@name.downcase}"]

      rules.push MfComment.new("installation rules for #{name}")

      rules.push MfRule.new("install_#{@name.downcase}",
                            Mkmf2.install_files_rules(@install_rule,
                                                      install_path,
                                                      @install_files),
                            ["$(#{@name.upcase}_SOURCE_FILES)",
                             "$(#{@name.upcase}_INSTALL_DIRS)"] 
                            )
    end

    # Rules for uninstalling the entity.
    # See install_rules for the return values.
    def uninstall_rules
      rules = ["uninstall_#{@name.downcase}"]

      rules.push MfComment.new("uninstallation rules for #{name}")
      files = [] # the rules to remove the installed files
      dirs = [] # the directories created during install
      @install_files.each_value do |v|
        dest = File.join(install_path,v)
        files.push Mkmf2.rule('remove',dest)
        dirs << File.dirname(dest)
      end
      dirs.uniq!
      for dir in dirs
        files << Mkmf2.rule('remove_path',dir)
      end
      rules.push MfRule.new("uninstall_#{@name.downcase}",
                            files)
    end

    # Rules for cleaning the source directory. See #install_rules for
    # an explanation about return values. This implementation does
    # nothing, as there is nothing to be built.
    def clean_rules
      return []
    end
      
    # Rules for building the entity. See #install_rules for
    # an explanation about return values.
    def build_rules
      return []
    end
    
    # Returns all the variables that should be added to the Makefile.
    # It should return a simple hash containing the values.
    def variables
      # we need to define this variable:
      ret = {}
      install = []
      source = []
      dirs = []
      @install_files.each do |k,f|
        file = File.join(install_path,f)
        install << file
        source << k
        dirs << File.dirname(file)
      end
      dirs.uniq! 
      ret["#{@name.upcase}_INSTALL_FILES"] = 
        Mkmf2.pretty_print_list(install,"#{@name.upcase}_INSTALL_FILES=")
      ret["#{@name.upcase}_INSTALL_DIRS"] = 
        Mkmf2.pretty_print_list(dirs,"#{@name.upcase}_INSTALL_DIRS=")
      ret["#{@name.upcase}_SOURCE_FILES"] = 
        Mkmf2.pretty_print_list(source,"#{@name.upcase}_SOURCE_FILES=")
      return ret
    end
  end

  # The class to create libraries that need to be built (that is basically
  # why I wrote the whole stuff !).
  class MfBinaryLibEntity < MfEntity

    # The source files of the library.
    attr_accessor :sources

    # The object files for the library
    attr_accessor :objects

    # The name of the file produced in the source directory.
    attr_accessor :target_file

    # The libraries on which the linking does depend
    attr_accessor :lib_depends

    # Supplementary command-line arguments for the linking step.
    # It will be used for instance to store library info.
    attr_accessor :suppl_args

    # +name+::  of the binary library produce, for instance "Biniou/Bidule"
    # +sources+:: sources of the files, defaults to all the potential
    #             +C+/+C\+\++ in the current directory.
    #             The sources are further expanded with Dir.glob.
    #             If you give then name of a directory, it will be
    #             expanded 
    # +target_file+:: how the files built in the current directory
    #                 will be called. Defaults to the same file name
    #                 as +name+.
    # +lib_depends+:: an array of objects, which designate ruby binary
    #                 libraries that this library does depend on;

    def initialize(name, sources = nil, target_file = nil,
                   lib_depends = nil)
      if sources.nil?
        sources = ["."]
      end

      @sources = []
      add_sources(sources)


      if target_file.nil?
        @target_file = File.basename(name) + ".#{CONFIG["DLEXT"]}"
      else
        @target_file = target_file
      end

      install_hash = { @target_file => name + ".#{CONFIG["DLEXT"]}" }
      # Then, we call the function of the parent
      super(name, install_hash,'bin_lib','install_bin')
      @suppl_args = ""
    end

    # Adds source files to the library. Can come in useful inside a block
    def add_sources(*sources)
      # we flatten the array, just to make sure everything is top-level
      sources.flatten!
      
      # We expand directory into the files in them
      # (no subdirs)
      sources.collect! do |f|
        if File.directory?(f)
          temp = []
          for ext in Mkmf2.source_files_extensions
            temp << File.join(f,"*.#{ext}")
          end
          temp
        else
          f
        end
      end

      # Then, we expand the files:
      for f in sources
        @sources += Dir.glob(f)
      end

      # and we update the list of current object files
      @objects = @sources.collect do |f|
        f.gsub(/\.[^.]+$/, ".#{CONFIG["OBJEXT"]}")
      end
      
    end
    # 
    def build_rules
      return [#"build_#{@name.downcase}",
              @target_file,
              # better to write this way... more clear on the
              # zsh command-line completion ;-) !
              MfRule.new(@target_file,
                         Mkmf2.rule('build_library', 
                                    "$(#{name.upcase}_OBJS)", 
                                    @suppl_args),
                         "$(#{name.upcase}_OBJS)")
             ]
    end

    # This removes the object files for the target...
    def clean_rules
      rules = ["clean_#{@name.downcase}"]

      rules.push MfComment.new("cleaning rules for #{name}")
      files = [] # the rules to clean the files
      @objects.each do |v|
        files.push Mkmf2.rule('remove',v)
      end

      files.push Mkmf2.rule('remove', @target_file)
      rules.push MfRule.new("clean_#{@name.downcase}",
                            files)
      return rules
    end

    def variables
      vars = super
      # We add the object list variable
      vars["#{name.upcase}_OBJS"] = 
        Mkmf2.pretty_print_list(@objects,"#{name.upcase}_OBJS=")
      return vars
    end
    
  end

  # Returns a list of the potential source files extensions.
  def Mkmf2.source_files_extensions
    return Mkmf2.cpp_files_extensions + 
      Mkmf2.c_files_extensions
  end 

  # Return the extensions for C++ files.
  def Mkmf2.cpp_files_extensions
    return %w{cpp cc cxx}
  end 

  # Return extensions for C files.
  def Mkmf2.c_files_extensions
    return %w{c}
  end 


  # This function declares a shared binary library that will be built
  # and installed using the appropriate functions.
  #
  # +libname+ is what is usually passed as an argument to the
  # +create_makefile+ function of the old 
  #
  # +sources+ is the source files. They are processed using Dir.glob, so
  # they can contain wildcards.
  #
  # This function contends itselfs to *declare* the library, it does not
  # *build* it, as this will be done by +make+.

  def declare_binary_library(libname, *sources, &b)
    add_entity(MfBinaryLibEntity.new(libname, sources), &b)
  end
  
  def add_entity(entity)
    yield entity if block_given?
    @@entities << entity
    return entity
  end

  # This function declares a Ruby library, namely files that want
  # to be installed directly in the rubylibdir (or equivalent, depending
  # on the user settings).
  # +target_dir+:: the target directory, relative to the installation
  #                directory;
  # +files+:: the files to install. Defaults to "lib/**.rb"
  #
  # For instance,
  #  declare_library("Biniou", "toto.rb")
  # will install the file "Biniou/toto.rb" in the common library
  # directory. Only basenames are kept for the target, so that
  #  declare_library("Biniou", "truc/muche/toto.rb")
  # does also end up in "Biniou/toto.rb". Beware !
  # If you think that this is obnoxious, tell me !
              
  def declare_library(target_dir,*files, &b)
    if files.empty?
      files << "lib/**/*.rb" 
    end
    declare_file_set(target_dir, files, 'lib', true, &b)
  end

  # Declares a set of files that should be executed directly be the user.
  def declare_exec(*files, &b)
    # No need for a target directory.
    declare_file_set("", files, 'exec', true, 'install_script',&b)
  end

  # Declares a documentation that doesn't need to be built. For the arguments,
  # see #declare_library.
  def declare_doc(target_dir, *files,&b)
    if files.empty?
      files << "doc/**/*"
    end
    declare_file_set(target_dir, files, 'doc', false,&b)
  end

  # Declares a set of include files to be installed. Defaults to
  # all the files in the include subdirectory.
  def declare_includes(target_dir,*files,&b)
    if files.empty?
      files << "include/**/*.h" 
    end
    declare_file_set(target_dir, files, 'include', true,&b)
  end
    
    

  # The main place for declaring files to be installed.
  # +target_dir+:: the target directory relative to the install path
  #                if it is nil or empty, then just install in the
  #                base directory.
  # +files+:: the source files
  # +kind+:: the kind of target (see again and again install_path)
  # +basename+:: wether or not to strip the leading directories ?
  # +skip+:: a regular expression (or whatever object that has a ===
  #          method that matches strings) saying wich files should
  #          be excluded. Defaults to /~$/.

  def declare_file_set(target_dir, files, kind = 'lib', 
                       basename = true, 
                       rule = 'install_data',
                       skip = /~$/, &b)
    source_files = []
    for glob in files
      source_files += Dir[glob]
    end
    install_hash = {}
    for f in source_files
      next if skip === f
      next if File.directory? f
      filename = if basename 
                   File.basename(f)
                 else
                   f
                 end
      if target_dir.nil? or target_dir.empty?
        install_hash[f] = filename
      else
        install_hash[f] = File.join(target_dir, filename) 
      end
    end
    add_entity(MfEntity.new(target_dir, install_hash, 
                            kind, rule), &b)
  end
  
  @@model = "local"

  # Sets the model for directory installation. There are for now 
  # three values:
  # local:: for a standard installation (like the default in mkmf.rb)
  # dist:: to use in a packageing system
  # home:: to install to a home directory (basically, prefix= <tt>$HOME</tt>)
  #
  # Please do not modify @@model directly.

  def Mkmf2.set_model(model = "local")
    @@model = model
  end

  # Sets up a whole bunch of variables necessary for installation, depending
  # on the current value of the @@model parameter. This is the place where we
  # setup the variables used by the installation process, namely:
  # +RUBYLIB_INSTALL_DIR+:: the directory where the text libraries
  #                         are to be installed;
  # +RUBYARCHLIB_INSTALL_DIR+:: the directory where architecture
  #                             dependent libraries are installed;
  # +INCLUDE_INSTALL_DIR+:: the directory where include files should go
  #                         if necessary;
  # And for now, that is all.
  def setup_model
    case @@model
    when "home"
      # I made a mistake for the home scheme.
      # we ensure the prefix is set to $HOME 
      MAKEFILE_CONFIG["RUBYLIB_INSTALL_DIR"] = 
        "$(DESTDIR)$(HOME)/lib/ruby"
      # For the HOME scheme, binaries and .rb files go
      # to the same directory.
      MAKEFILE_CONFIG["RUBYARCHLIB_INSTALL_DIR"] = 
        "$(DESTDIR)$(HOME)/lib/ruby"
      MAKEFILE_CONFIG["INCLUDE_INSTALL_DIR"] = 
        "$(DESTDIR)$(HOME)/include"
      MAKEFILE_CONFIG["EXEC_INSTALL_DIR"] = 
        "$(DESTDIR)$(HOME)/bin"
        
    when "dist"
      # shares some code with the previous item, don't forget to
      # update *BOTH* !
      MAKEFILE_CONFIG["RUBYLIB_INSTALL_DIR"] = 
        "$(DESTDIR)$(rubylibdir)"
      MAKEFILE_CONFIG["RUBYARCHLIB_INSTALL_DIR"] = 
        "$(DESTDIR)$(archdir)"
      MAKEFILE_CONFIG["INCLUDE_INSTALL_DIR"] = 
        "$(DESTDIR)$(includedir)"
      MAKEFILE_CONFIG["EXEC_INSTALL_DIR"] = 
        File.join("$(prefix)", "bin")

    when "local"
      MAKEFILE_CONFIG["RUBYLIB_INSTALL_DIR"] = 
        "$(sitelibdir)"
      MAKEFILE_CONFIG["RUBYARCHLIB_INSTALL_DIR"] = 
        "$(sitearchdir)"

      # For the include directory, the strategy to use is far less clear.
      # I propose to strip the last two directories
      # from MAKEFILE_CONFIG["sitelibdir"] and append 
      # 'include' then. Whether this is a good idea, I don't know.

      basedir = File.dirname(File.dirname(MAKEFILE_CONFIG["sitedir"]))
      MAKEFILE_CONFIG["INCLUDE_INSTALL_DIR"] = 
        File.join(basedir,"include")
      MAKEFILE_CONFIG["EXEC_INSTALL_DIR"] = 
        File.join(basedir,"bin")
    end

  end


  # Returns the installation path for a given thing (+what+). This function
  # basically returns a simple MAKEFILE_CONFIG variable, which has otherwise
  # been setup by setup_model. What is available for now:
  # +lib+:: where to install .rb library
  # +bin_lib+:: where to install binary libs
  # +include+:: include files

  def Mkmf2.install_path(what)
    # we should definitely append a $(DESTDIR) to every single
    # directory we return, since it will make debugging
    
    case what
    when 'lib'
      return Mkmf2.config_var('RUBYLIB_INSTALL_DIR')
    when 'exec'
      return Mkmf2.config_var('EXEC_INSTALL_DIR')
    when 'bin_lib'
      return Mkmf2.config_var('RUBYARCHLIB_INSTALL_DIR')
    when 'include'
      return Mkmf2.config_var('INCLUDE_INSTALL_DIR')
    else
      raise "Unkown installation directory specification: #{what}"
    end 
  end

  @@registered_names = {} # a hash containing already registered names
  # to avoid namespace clobbering.
  
  # Register a new name, making sure the name returned is unique.
  def Mkmf2.register_name(name)
    # first, transform all that is not letter into underscore
    name.gsub!(/[^a-zA-Z]/,'_')
    name.downcase!
    # then, increment the number
    if @@registered_names[name]
      i = 1
      while @@registered_names["#{name}_#{i}"]
        i += 1
      end
      name = "#{name}_#{i}"
    end
    @@registered_names[name] = true
    return name
  end
  

  # Stores which configuration variables are used
  @@config_variables_used = []
  # Returns the given config key, that will be used to write a rule
  # in the Makefile. For better output, the function does store a list of
  # all the config variables which are in use, and simply outputs 
  # +$(VARIABLE)+. 
  def Mkmf2.config_var(str)
    @@config_variables_used << str
    return "$(#{str})"
  end

  # Returns a string containing all the configuration variables. We
  # use the MAKEFILE_CONFIG for more flexibility in the Makefile: the
  # variables can then be redefined on the make command-line.
  def Mkmf2.output_config_variables
    str = ""
    keys = @@config_variables_used.uniq
    new_keys = []
    begin 
      # we merge the new keys with the old
      keys += new_keys
      keys.uniq!
      new_keys = []
      keys.each do |k|
        if MAKEFILE_CONFIG.key? k
          MAKEFILE_CONFIG[k].gsub(/\$\((\w+)\)/) { |k|
            # we add the key to the list only if it exists, else the
            # environment variable gets overridden by what is written
            # in the Makefile (for $HOME, for instance)
            new_keys << $1 if MAKEFILE_CONFIG.key? $1
          }
        end
      end
    end until (keys + new_keys).uniq.length == keys.length
    
    for var in keys.uniq.sort
      # We output the variable only if it is not empty: makes
      # it a lot easier to modify them from outside...
      if MAKEFILE_CONFIG[var] =~ /\S/
        str += "#{var}=#{MAKEFILE_CONFIG[var]}\n"
      end
    end
    return str
  end
    

  # This hash contains replacements for basic filesystem functions
  # based on ruby and FileUtils. They are used only if the corresponding
  # element is missing in CONFIG. They also contains some other default
  # values for possibly missing CONFIG keys.
  CONFIG_DEFAULTS = {
    # first, a short for the rest...
    "RB_FILE_UTILS" => "$(RUBY_INSTALL_NAME) -r fileutils", 
    "INSTALL_SCRIPT" => 
    "$(RB_FILE_UTILS) -e 'FileUtils.install(ARGV[0],ARGV[1],:mode => 0755)'",
    "INSTALL_DATA" => 
    "$(RB_FILE_UTILS) -e 'FileUtils.install(ARGV[0],ARGV[1],:mode => 0644)'",
    "INSTALL_PROGRAM" =>  "$(INSTALL_SCRIPT) ",
    "MAKEDIRS" => 
    "$(RB_FILE_UTILS) -e 'FileUtils.makedirs(ARGV)'",
    "RM" => 
    "$(RB_FILE_UTILS) -e 'FileUtils.rm(ARGV)'",
    "RM_PATH" => # a trick to remove a path
    "$(RB_FILE_UTILS) -e 'd = ARGV[0]; begin ;while FileUtils.rmdir d, :verbose=>true; d = File.dirname(d);end; rescue ; end;'",
    "DEFINES" => "",
    "LIBARG" => "-l%s",
    "LIBS_SUP" => "", 
  } 
  
  # This functions checks for missing features in the CONFIG hash and 
  # supplements them using the FILE_UTILS_COMMAND hash if necessary.
  def check_missing_features
    CONFIG.delete("INSTALL_SCRIPT")   # Not consistent, it's better to use
    # the Fileutils stuff
    for key, val in CONFIG_DEFAULTS 
      if CONFIG.key?(key) and CONFIG[key] =~ /\w/
        # everything is fine
      else
        # we supplement the feature.
        MAKEFILE_CONFIG[key] = val
      end
    end
  end

  # Takes a list of CONFIG variables and returns them joined by spaces. 
  def Mkmf2.config_join(*vars)
    return vars.collect { |v|
      Mkmf2.config_var(v)
    }.join(' ')
  end
    

  # Returns the way to represent the dependencies of a rule in the
  # Makefile.
  def Mkmf2.mf_deps
    return " $(@D)"
  end

  # Returns the way to represent the current target in the Makefile.
  def Mkmf2.mf_target
    return " $@"
  end

  # A small helper function to write quickly rules, based on a configuration
  # item.
  def Mkmf2.mf_rule(cfg,*args)
    if args.empty?
      return Mkmf2.config_var(cfg) + 
        Mkmf2.mf_deps + Mkmf2.mf_target
    else
      return Mkmf2.config_var(cfg) + ' ' + args.join(' ')
    end
  end

  # This function returns the common build rules for C and C++ files.
  def Mkmf2.common_build_rules
    rules = []
    for ext in Mkmf2.c_files_extensions
      rules << MfRule.new(".#{ext}.#{CONFIG["OBJEXT"]}",
                          Mkmf2.config_join("CC",
                                            "CFLAGS",
                                            "CPPFLAGS",
                                            "INCLUDEDIRS",
                                            "DEFINES") +
                          " -c $< #{CONFIG["OUTFLAG"]} $@" 
                          ) 
    end 

    # The same, but with C++:
    for ext in Mkmf2.cpp_files_extensions
      rules << MfRule.new(".#{ext}.#{CONFIG["OBJEXT"]}",
                          Mkmf2.config_join("CXX",
                                            "CXXFLAGS",
                                            "CPPFLAGS",
                                            "INCLUDEDIRS",
                                            "DEFINES") +
                          " -c $< #{CONFIG["OUTFLAG"]} $@" 
                          ) 
    end 
    return rules
  end 

  # Now, the infrastructure for dealing with include and library
  # directories:
  @@include_path = [Mkmf2.config_var("rubylibdir"),
                    Mkmf2.config_var("archdir"),
                    '.',
                    File.join('.','include')
                   ]

  
  # Adds one or more +paths+ to the current include path.
  def add_include_path(*paths)
    @@include_path += paths.flatten
  end

  def output_include_path
    return @@include_path.collect {|v|
      "-I#{v}"
    }.join(' ')
  end

  @@library_path = []

  # Adds one or more +paths+ to the current library path.
  def add_library_path(*paths)
    @@library_path += paths.flatten
  end

  def output_library_path
    return @@library_path.collect {|v|
      "-L#{v}"
    }.join(' ')
  end

  # I know this is bad design, but that's the best I think of for now.
  # This function better be called before using MAKEFILE_CONFIG directly.
  def update_makefile_config
    setup_paths_variables
  end

  # Sets up the various variables pertaining to include
  # and library paths.
  def setup_paths_variables
    MAKEFILE_CONFIG["INCLUDEDIRS"] = output_include_path
    MAKEFILE_CONFIG["LIBDIRS"] = output_library_path
  end

  # A recommandation for line size in the Makefile
  MAKEFILE_LINE_SIZE = 40
  
  # Returns a string where all the elements are joined together. The lines
  # will break if they exceed +line_size+, but the elements themselves
  # will not be broken. They will be indented
  def Mkmf2.pretty_print_list(list, indent = 0,
                              line_size = MAKEFILE_LINE_SIZE)
    if indent.is_a? String
      indent = indent.length
    end
    lines = []
    for elem in list
      if lines.last.nil? || 
          (lines.last + elem).length > line_size
        lines << elem.dup # necessary to force ruby to create
      else
        lines.last.concat(' ' + elem)
      end
    end
    return lines.join("\\\n#{' ' * indent}")
  end

  
  # Returns the string corresponding to a rule, given by the string +rule+,
  # which can take the following values:
  # +install_data+::   returns the rule for installing a data file to a
  #                    given place
  # +install_script+:: the rule for installing code at a given place
  # +remove+::         to remove files
  # +directory+:: to create a directory
  # +build_library+:: to build a library; in that case, the first argument
  #                   is the name of the target, and the ones after
  #                   the objects.
  #
  # the optional arguments are the arguments to the rule, if they exist. Else,
  # they default to $(@D) $@ (or something like that).
  
  def Mkmf2.rule(rule, *args)
    case rule
    when 'install_data'
      return Mkmf2.mf_rule("INSTALL_DATA",args)
    when 'install_script'
      return Mkmf2.mf_rule("INSTALL_SCRIPT",args)
    when 'install_bin'
      return Mkmf2.mf_rule("INSTALL_PROGRAM",args)
    when 'remove'
      return "-" + Mkmf2.mf_rule("RM",args)
    when 'remove_path'
      return "-" + Mkmf2.mf_rule("RM_PATH", args)
    when 'directory' # to create a directory
      return Mkmf2.mf_rule("MAKEDIRS", args)
    when 'build_library' 
      return Mkmf2.mf_rule("LDSHARED",
                           Mkmf2.config_join("DLDFLAGS",
                                             "LIBDIRS"),
                           CONFIG["OUTFLAG"],
                           "$@", # the target,
                           args, # and the source
                           # The libraries should better come in the end,
                           # since for instance the standard linux ld
                           # does only link with the symbols that have been
                           # reported missing in the previous files.
                           Mkmf2.config_join("LIBRUBYARG_SHARED", 
                                             # we shouldn't forget this one !!
                                             "LOCAL_LIBS",
                                             "LIBS",
                                             # and then, the global libraries
                                             # that have been added using
                                             # have_library
                                             "LIBS_SUP")
                           )
    end
  end
  
  @@directories = {}
  # Register a directory that we might need to create. Make sure that
  # the rules don't appear twice.
  def Mkmf2.register_dir(dir)
    if ! @@directories.key?(dir)
      @@directories[dir] = true
    end
  end

  # Return the rules to create the necessary directories
  def directory_rules
    rules = []
    for dir in @@directories.keys 
      rules << MfRule.new(dir, Mkmf2.rule('directory',dir))
    end
    return rules
  end


  # A small helper function to extract the install directory name
  # for one file. +file+ is the file, +install_dir+ where we want to
  # install it. This should take care of messy dots.
  def Mkmf2.dest_dir(file, install_dir)
      dir = File::dirname(file)
      if dir == "."
        return install_dir
      else
        return File.join(install_dir,dir)
      end
  end

  # A helper function for MfEntity instances that will have to install
  # files into directories.
  # +rule+:: the rule to use, see Mkmf2::rule
  # +install_path+:: the installation path
  # +files+:: a hash containing original -> destination pairs.

  def Mkmf2.install_files(rule, install_path,files)
    rules = []
    files.each do |k,v|
      destdir = Mkmf2.dest_dir(v, install_path)
      Mkmf2.register_dir(destdir)
      rules.push(MfRule.new(File.join(install_path,v),
                            Mkmf2::rule(rule, k,
                                        File.join(install_path,v)),
                            [k,destdir])) 
      # depends on both the file
      # that will be installed and the directory where to install it.
    end

    return rules
  end

  # The companion of install_files, returning a list of strings
  # rather than MfRules. Bascially behaves the same way. It is probably
  # a better thing to use this function now.
  def Mkmf2.install_files_rules(rule, install_path,files)
    rules = []
    files.each do |k,v|
      dir = File::dirname(v)
      Mkmf2.register_dir(Mkmf2.dest_dir(v,install_path))
      rules.push Mkmf2::rule(rule, k, File.join(install_path,v))
      
      # depends on both the file
      # that will be installed and the directory where to install it.
    end

    return rules
  end

  # Small helper function for write_makefile. +target+ is
  # a hash containing a "deps" array and a "rules" array
  # which are created in case of need.
  def unwrap_rules(target, source)
    # we first make sure that the keys exist, even if we don't
    # have anything to append to them.
    if ! target.has_key? "deps"
      target["deps"] = []
    end
    if ! target.has_key? "rules"
      target["rules"] = []
    end

    return if source.empty?
    target["deps"] << source.shift
    target["rules"] += source
  end

  # Writes the Makefile using @@entities -- and others...
  def write_makefile(file_name = "Makefile")
    install = {}
    uninstall = {}
    build = {}
    clean = {}
    vars = []

    setup_model


    for entity in @@entities
      t = entity.install_rules
      unwrap_rules(install,t)

      t = entity.uninstall_rules
      unwrap_rules(uninstall,t)
      
      t = entity.build_rules
      unwrap_rules(build,t)

      t = entity.clean_rules
      unwrap_rules(clean,t)

      vars << entity.variables
    end

    # Common rules: 
    common = Mkmf2.common_build_rules()
    
    # Setup the path variables:
    setup_paths_variables
    dir_rules = directory_rules()

    # OK, now, everything is prepared, we just need to create the
    # makefile and output everything into it...

    f = open(file_name, File::WRONLY|File::CREAT|File::TRUNC)


    # First, the variables in use:
    f.puts Mkmf2.output_config_variables

    # build has to be the first target so that simply
    # invoking make does the building, but not the installing.
    # now, the main rules
    f.puts "# main rules"
    # we force the dependence on build for install so that
    # ruby library files don't get installed before the c code is
    # compiled...
    # build is output first so that invoking make without
    # arguments builds.
    f.print MfRule.new("build", nil, build["deps"]).to_s
    f.print MfRule.new("install", nil, ["build"] + install["deps"]).to_s
    f.print MfRule.new("uninstall", nil, uninstall["deps"]).to_s
    f.print MfRule.new("clean", 
                       [Mkmf2.rule('remove',"**/*~")], # remove archive files
                       # by default in the clean target.
                       clean["deps"]).to_s

    # Add a distclean rule, to make debuild happy.
    f.print <<"EOR"
distclean: clean 
\t@-$(RM) Makefile extconf.h conftest.* mkmf.log
\t@-$(RM) core ruby$(EXEEXT) *~ $(DISTCLEANFILES) 
EOR

    # Phony targets:
    f.puts ".PHONY: build install uninstall"

    f.puts "# Common rules:"
    common.each {|v|
      f.print v.to_s
    }

    f.puts ""
    
    # We write the variables:
    f.puts "# entities-dependent variables"
    for v in vars 
      v.each do |k,v|
        f.print "#{k}=#{v}\n"
      end
    end

    f.puts "# Rules to make directories"

    # directory rules
    for rule in dir_rules
      f.print rule.to_s
    end

    f.puts "# entities-dependent rules"
    # We write the rules
    for rule in (install['rules'] + 
                 uninstall['rules'] +
                 build['rules'] + clean['rules'])
      f.print rule.to_s
    end

    if @@user_rules.length > 0
      f.puts "\n# User-defined rules"
      for rule in @@user_rules
        f.puts rule.to_s
      end
    end


    f.close # not necessary, but good practice ;-) ?? 

  end

  # Adds a custom rule to the Makefile. If only the first argument
  # is specified, it is written as is to the Makefile. If at least the
  # second or the third is non-nil, a MfRule is created with the arguments
  # and written to the Makefile.
  def custom_rule(str, rule = nil, deps = nil)
    if rule || deps 
      @@user_rules  << MfRule.new(str, rule, deps)
    else
      @@user_rules << str
    end
  end

  # *The* compatibility function with the mkmf.rb !
  def create_makefile(target)
    declare_library(File.dirname(target))
    declare_binary_library(target, "**/*.c")

    write_makefile
  end

  # A function to ease the task of producing several libraries from the
  # same source tree. It sets up a library context for one directory,
  # adding it to the include path, and taking care of the files in:
  # +lib/+:: the ruby library files;
  # +include/+:: the include files
  #
  # +dir+ is the directory in the source, +target_dir+ the base directory
  # for target files. It is pretty rudimentary, but should do the job
  # for many cases (and especially Tioga ;-) !). +name+ is the name
  # of the binary library. If nil, doesn't try to build a binary library.
  # +include_dir+ is the target name for includes. If +nil+, doesn't
  # try to install them.
  #
  # You can use either the return value or a block to change somehow the
  # behavior of the different objects produced (like adding a binary library
  # for instance).
  
  def setup_dir(dir, target_dir, bin_name = nil,
                include_dir = nil)
    add_include_path(dir, File.join(dir, 'include'))

    lib = declare_library(target_dir, "#{dir}/lib/**/*.rb")
    binlib = declare_binary_library(bin_name, 
                                    "#{dir}/**/*.c") if ! bin_name.nil?
    include = declare_includes(include_dir, 
                     "#{dir}/include/**/*.h") if ! include_dir.nil?
    yield lib, binlib, include if block_given?
    [lib, binlib, include]
  end

  # the module variable that will hold the command-line arguments:
  # even with a similar name, the contents of this variable will
  # be quite different from the old $configure_args
  @@configure_args = {}

  # Parses command-line arguments, wiping the ones that we understood.
  def parse_cmdline
    $*.delete_if do |arg|
      if arg =~ /^--(.*)/ # does look like a command-line argument...
        case $1
        when "local" # installation to sitedir
          Mkmf2.set_model("local")
          true
        when "dist"
          Mkmf2.set_model("dist")
          true
        when "home"
          Mkmf2.set_model("home")
          true
        when /^--with(.*)/ # this is my understanding of the
          # with-config stuff. It is a complete rewrite, I don't like
          # much the old code
          rhs = $1 # right-hand side
          case rhs
          when /-([\w-]+)=(.*)/
            @@configure_args[$1] = $2
          when /out-([\w-]+)$/
            @@configure_args[$1] = false
          when /-([\w-]+)$/
            @@configure_args[$1] = true
          else
            raise "Argument #{arg} not understood"
          end
        else
          false
        end
      else
        false
      end
    end
  end

  # This function is a try at reproducing the functionnality of the
  # old mkmf.rb's dir_config. It is not a copy, and might not work
  # well in all situations. Basically, if --with-target-dir has been
  # specified, use dir/include dir/lib. If --with-target-(include|lib)
  # have been specified, use them !
  def dir_config(target)
    ldir = nil
    idir = nil
    if @@configure_args.key?("#{target}-lib") 
      ldir = @@configure_args["#{target}-lib"]
    elsif @@configure_args.key?("#{target}-dir") 
      ldir = File::join(@@configure_args["#{target}-dir"], "lib")
    end
    if ldir
      add_library_path(ldir.split(File::PATH_SEPARATOR))
    end

    if @@configure_args.key?("#{target}-include") 
      idir = @@configure_args["#{target}-include"]
    elsif @@configure_args.key?("#{target}-dir") 
      idir = File::join(@@configure_args["#{target}-dir"], "include")
    end
    if idir
      add_include_path(idir.split(File::PATH_SEPARATOR))
    end
    return [idir, ldir]
  end

  ###########################################################################
  # This module is copied verbatim from the old mkmf.rb code. It comes dead #
  # useful for logging things. I hope it will not cause licence conflicts.  #
  ###########################################################################

  module Logging
    @log = nil
    @logfile = 'mkmf.log'
    @orgerr = $stderr.dup
    @orgout = $stdout.dup
    @postpone = 0
    
    def self::open
      @log ||= File::open(@logfile, 'w')
      @log.sync = true
      $stderr.reopen(@log)
    $stdout.reopen(@log)
      yield
    ensure
      $stderr.reopen(@orgerr)
      $stdout.reopen(@orgout)
    end
    
    def self::message(*s)
      @log ||= File::open(@logfile, 'w')
      @log.sync = true
      @log.printf(*s)
    end
    
    def self::logfile file
      @logfile = file
      if @log and not @log.closed?
        @log.flush
        @log.close
        @log = nil
      end
    end
    
    def self::postpone
      tmplog = "mkmftmp#{@postpone += 1}.log"
      open do
        log, *save = @log, @logfile, @orgout, @orgerr
        @log, @logfile, @orgout, @orgerr = nil, tmplog, log, log
        begin
          log.print(open {yield})
          @log.close
          File::open(tmplog) {|t| FileUtils.copy_stream(t, log)}
        ensure
          @log, @logfile, @orgout, @orgerr = log, *save
          @postpone -= 1
          rm_f tmplog
        end
      end
    end
  end

  # Also from old mkmf.rb
  CONFTEST_C = "conftest.c"

  def xsystem(command)
    Logging::open do
      puts command.quote
      system(command)
    end
  end
  
  # Also from old mkmf.rb
  def log_src(src)
    Logging::message <<"EOM", src
checked program was:
/* begin */
%s/* end */

EOM
  end
  
  # Also from old mkmf.rb
  def create_tmpsrc(src)
    src = yield(src) if block_given?
    src = src.sub(/[^\n]\z/, "\\&\n")
    open(CONFTEST_C, "wb") do |cfile|
      cfile.print src
    end
    src
  end
  
  # Also from old mkmf.rb
  def try_do(src, command, &b)
    src = create_tmpsrc(src, &b)
    xsystem(command)
  ensure
    log_src(src)
  end

  # Also from old mkmf.rb
  def checking_for(m, fmt = nil)
    f = caller[0][/in `(.*)'$/, 1] and f << ": " #` for vim
    m = "checking for #{m}... "
    print m
    a = r = nil
    Logging::postpone do
      r = yield
      a = (fmt ? fmt % r : r ? "yes" : "no") << "\n"
      "#{f}#{m}-------------------- #{a}\n"
    end
    puts a
    $stdout.flush
    Logging::message "--------------------\n\n"
    r
  end

  # also taken straight from mkmf.rb
  def rm_f(*files)
    FileUtils.rm_f(Dir[files.join("\0")])
  end

  # A small wrapper around Config::expand which diminishes the size of the code
  # and makes sure the MAKEFILE_CONFIG hash is updated.
  def expand_vars(str)
    update_makefile_config
    string = Config::expand(str,MAKEFILE_CONFIG)
    # then, we need to turn all the remaining $(THING) into $THING, so that
    # the shell doesn't spawn subshells ? 
    return string.gsub(/\$\((\w+)\)/) { "$#$1" }
  end

  # Add defines to the build
  def add_define(d)
        MAKEFILE_CONFIG["DEFINES"] += 
          " -D_#{d}"
  end

  # This is a compatibility function with the previous mkmf.rb. It does check
  # for the presence of a header in the current include directories. If found,
  # it returns true and sets the define HAVE_...
  def have_header(header, &b)
    checking_for header do
      if try_do("#include <#{header}>",
                expand_vars("$(CPP) $(INCLUDEDIRS) " \
                            "$(CPPFLAGS) $(CFLAGS) $(DEFINES)"\
                            "#{CONFTEST_C} $(CPPOUTFILE)"),&b)
        add_define("HAVE_#{header.sanitize}")
        true
      else
        false
      end
    end
  ensure
    rm_f("conftest*")
  end

  # The same as have_header, but fails if the header is not found...
  def require_header(header, message = nil ,&b)
    if ! have_header(header,&b)
      if message
        puts message
      end
      raise "Header #{header} not found, stopping\n"
    end
  end

  def mkmf2_init
    check_missing_features
    parse_cmdline
  end

  # Compatibility function from mkmf.rb. Checks if the compiler
  # can find the given function in the given library. If the function
  # is not given, we look for main but it's definitely not a good idea.
  # +header+ is a header that can be included to get the prototype for
  # this function. It can possibly be an array, in which case it is
  # interpreted as a list of headers that should be included.

  # code still needs to be done...
  def have_library(lib, func = nil, header=nil, &b)
    if func.nil?
      func = "main"
    end
    headers = ""
    if header
      if header.is_a?(Array) 
        header.each {|h|
          headers += "#include <#{h}>\n"
        }
      else
        headers += "#include <#{header.to_s}>\n"
      end
    end
    libarg = "#{MAKEFILE_CONFIG["LIBARG"]%lib}"
    checking_for "#{func}() in #{libarg}" do
      code = <<"SRC"
#{headers}
/*top*/
int main() { return 0; }
int t() { void ((*volatile p)()); p = (void ((*)()))#{func}; return 0; }
SRC
      if try_do(code,
                expand_vars("$(CC) $(OUTFLAG)conftest $(INCFLAGS) " +
                            "#{CONFTEST_C} " + 
                            " -I$(hdrdir) $(CPPFLAGS) $(CFLAGS) $(src)" + 
                            " $(LIBPATH) $(LDFLAGS) $(ARCH_FLAG) " +
                            " $(LOCAL_LIBS) $(LIBS) $(LIBS_SUP) #{libarg}"
                            )
                )
                
        add_define("HAVE_#{lib.sanitize}")
        MAKEFILE_CONFIG["LIBS_SUP"] += " #{libarg}"
        true
      else
        false
      end
    end
  ensure
    rm_f("conftest*")
  end
end

include Mkmf2
mkmf2_init
