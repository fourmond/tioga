# history_logs.rb

class HistoryLogs

    # basics log
    attr_accessor :star_Age, :time_Step, :log_Luminosity, :log_Radius
    attr_accessor :log_surface_Temp, :log_center_Temp, :log_center_Density
    attr_accessor :log_center_Pressure, :center_Degeneracy, :center_H
    attr_accessor :center_He, :center_C, :center_N, :center_O, :center_Ne
    attr_accessor :dynamic_Timescale, :thermal_Timescale, :nuclear_Timescale
    
    # masses log
    attr_accessor :star_Mass, :star_Mdot, :star_Mass_H, :star_Mass_He
    attr_accessor :star_Mass_C, :star_Mass_N, :star_Mass_O
    attr_accessor :star_Mass_Ne, :mass_He_Core, :mass_C_Core, :mass_O_Core
    
    # pressure log
    attr_accessor :center_PRAD, :center_PEL, :center_PION, :center_PCORR

    # power log
    attr_accessor :power_H_burn, :power_He_burn, :power_Metal_burn
    attr_accessor :power_PP, :power_CNO, :power_3_alpha
    attr_accessor :power_Neutrinos, :power_plasmon_neutrinos, :power_brem_neutrinos
    attr_accessor :power_pair_neutrinos, :power_photo_neutrinos
    
    # burn log
    attr_accessor :burn_H_R_1, :burn_H_R_2, :burn_H_R_3, :burn_H_R_4
    attr_accessor :burn_H_M_1, :burn_H_M_2, :burn_H_M_3, :burn_H_M_4
    attr_accessor :burn_He_R_1, :burn_He_R_2, :burn_He_R_3, :burn_He_R_4
    attr_accessor :burn_He_M_1, :burn_He_M_2, :burn_He_M_3, :burn_He_M_4

    # convection log
    attr_accessor :conv_R_s1, :conv_R_e1, :conv_R_s2, :conv_R_e2, :conv_R_s3, :conv_R_e3
    attr_accessor :conv_M_s1, :conv_M_e1, :conv_M_s2, :conv_M_e2, :conv_M_s3, :conv_M_e3
    attr_accessor :inner_CZM, :inner_CZR, :inner_MZM, :inner_MZR
    
    # profiles log
    attr_accessor :profile_Ages, :profile_Mass, :profile_MODNUM
    
    # misc other stuff
    attr_accessor :net_He_power, :num_models, :num_profiles
        
    def initialize(path = nil)
        @basics_log = [ # columns in basics.log
            @star_Age = Dvector.new,
            @time_Step = Dvector.new,
            @log_Luminosity = Dvector.new,
            @log_Radius = Dvector.new,
            @log_surface_Temp = Dvector.new,
            @log_center_Temp = Dvector.new,
            @log_center_Density = Dvector.new,
            @log_center_Pressure = Dvector.new,
            @center_Degeneracy = Dvector.new,
            @center_H = Dvector.new,
            @center_He = Dvector.new,
            @center_C = Dvector.new,
            @center_N = Dvector.new,
            @center_O = Dvector.new,
            @center_Ne = Dvector.new,
            @dynamic_Timescale = Dvector.new,
            @thermal_Timescale = Dvector.new,
            @nuclear_Timescale = Dvector.new ]
        @masses_log = [ # columns in masses.log
            nil, # star_Age
            @star_Mass = Dvector.new,
            @star_Mdot = Dvector.new,
            @star_Mass_H = Dvector.new,
            @star_Mass_He = Dvector.new,
            @star_Mass_C = Dvector.new,
            @star_Mass_N = Dvector.new,
            @star_Mass_O = Dvector.new,
            @star_Mass_Ne = Dvector.new,
            @mass_He_Core = Dvector.new,
            @mass_C_Core = Dvector.new,
            @mass_O_Core = Dvector.new ]
        @pressure_log = [ # columns in pressure.log
            nil, # star_Age
            @center_PRAD = Dvector.new,
            @center_PEL = Dvector.new,
            @center_PION = Dvector.new,
            @center_PCORR = Dvector.new ]
        @power_log = [ # columns in power.log
            nil, # star_Age
            @power_H_burn = Dvector.new,
            @power_He_burn = Dvector.new,
            @power_Metal_burn = Dvector.new,
            @power_PP = Dvector.new,
            @power_CNO = Dvector.new,
            @power_3_alpha = Dvector.new,
            @power_Neutrinos = Dvector.new,
            @power_plasmon_neutrinos = Dvector.new,
            @power_brem_neutrinos = Dvector.new,
            @power_pair_neutrinos = Dvector.new,
            @power_photo_neutrinos = Dvector.new ]
        @burn_log = [ # columns in burn.log
            nil, # star_Age
            @burn_H_R_1 = Dvector.new,
            @burn_H_R_2 = Dvector.new,
            @burn_H_R_3 = Dvector.new,
            @burn_H_R_4 = Dvector.new,
            @burn_H_M_1 = Dvector.new,
            @burn_H_M_2 = Dvector.new,
            @burn_H_M_3 = Dvector.new,
            @burn_H_M_4 = Dvector.new,
            @burn_He_R_1 = Dvector.new,
            @burn_He_R_2 = Dvector.new,
            @burn_He_R_3 = Dvector.new,
            @burn_He_R_4 = Dvector.new,
            @burn_He_M_1 = Dvector.new,
            @burn_He_M_2 = Dvector.new,
            @burn_He_M_3 = Dvector.new,
            @burn_He_M_4 = Dvector.new ]
        @convection_log = [ # columns in convection.log
            nil, # star_Age
            @conv_R_s1 = Dvector.new,
            @conv_R_e1 = Dvector.new,
            @conv_R_s2 = Dvector.new,
            @conv_R_e2 = Dvector.new,
            @conv_R_s3 = Dvector.new,
            @conv_R_e3 = Dvector.new,
            @conv_M_s1 = Dvector.new,
            @conv_M_e1 = Dvector.new,
            @conv_M_s2 = Dvector.new,
            @conv_M_e2 = Dvector.new,
            @conv_M_s3 = Dvector.new,
            @conv_M_e3 = Dvector.new,
            @inner_CZM = Dvector.new,
            @inner_CZR = Dvector.new,
            @inner_MZM = Dvector.new,
            @inner_MZR = Dvector.new ]
        @profiles_log = [ # columns in profiles.log
            @profile_Ages = Dvector.new,
            @profile_Mass = Dvector.new,
            @profile_MODNUM = Dvector.new ]
        @net_He_power = Dvector.new
        read_logs(path) if path != nil
        return self
    end
    
    def read_logs(path=nil)
        path = '' if path == nil
        path = path + '/' if path.length > 0 && path[-1..-1] != '/'
        Dvector.read(path + 'basics.log', @basics_log)
        Dvector.read(path + 'masses.log', @masses_log)
        Dvector.read(path + 'pressure.log', @pressure_log)
        Dvector.read(path + 'power.log', @power_log)
        Dvector.read(path + 'burn.log', @burn_log)
        Dvector.read(path + 'convection.log', @convection_log)
        Dvector.read(path + 'profiles.log', @profiles_log)
        @num_models = @star_Age.length
        @num_profiles = @profile_Ages.length
        @net_He_power.replace(@power_He_burn).plus!(@power_Metal_burn).sub!(@power_Neutrinos)
    end

end
