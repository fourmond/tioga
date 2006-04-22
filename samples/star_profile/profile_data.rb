# profile_data.rb

class ProfileData

    include Math
    include Tioga
    include FigureConstants
    
    attr_accessor :model_Number, :star_Age, :time_Step # 1
    attr_accessor :log_Luminosity, :log_Radius, :log_surface_Temp, :log_center_Temp, :log_center_Density # 2
    attr_accessor :log_center_Pressure, :center_Degeneracy, :center_H, :center_He # 3
    attr_accessor :center_C, :center_N, :center_O, :center_Ne # 4
    attr_accessor :center_Mg, :center_Si, :center_Fe # 5
    attr_accessor :initial_Mass, :star_Mass, :star_Mdot, :initial_Z # 6
    attr_accessor :star_Mass_H, :star_Mass_He, :star_Mass_C, :star_Mass_N # 7
    attr_accessor :star_Mass_O, :star_Mass_Ne, :star_Mass_Mg, :star_Mass_Si, :star_Mass_Fe # 8
    attr_accessor :mass_He_Core, :mass_C_Core, :mass_O_Core # 9
    attr_accessor :dynamic_Timescale, :thermal_Timescale, :nuclear_Timescale # 10
    attr_accessor :power_H_burn, :power_He_burn, :power_Metal_burn, :power_Neutrinos # 11
    attr_accessor :power_PP, :power_CNO, :power_3_alpha, :power_C_alpha, :power_N_alpha, :power_O_alpha, :power_Ne_alpha # 12
    attr_accessor :power_CC_Ne, :power_CO, :power_OO, :power_Ne_decay, :power_Mg_decay, :power_CC_Mg # 13
    attr_accessor :power_plasmon_neutrinos, :power_brem_neutrinos, :power_pair_neutrinos, :power_photo_neutrinos # 14
    
    @@num_header_lines = 14
    
    attr_accessor :sx_R # radius (Rsolar)
    attr_accessor :sx_T # temperature (K)
    attr_accessor :sx_M # mass interior to this meshpoint (Msolar)
      # this is automatically adjusted during the run to best allocate a fixed number of meshpoints
    attr_accessor :sx_L
       # luminosity (Lsolar) -- net rate of outward energy flow at this meshpoint (nuclear plus thermal minus neutrino)
       # this can be negative as a result of neutrino cooling at the center
    attr_accessor :sx_PSI # the degeneracy parameter (psi is the same as defined in Kippenhahn & Weigert)
       # psi replaces density or pressure as an independent variable
    
    
    @@num_shells = 199

    # composition variables -- all are local mass fractions
    attr_accessor :sx_XH # H1
    attr_accessor :sx_XHE # He4
    attr_accessor :sx_XC # C12
    attr_accessor :sx_XN # N14
    attr_accessor :sx_XO # O16
    attr_accessor :sx_XNE # Ne20
    attr_accessor :sx_XMG # Mg24

    # some of the major dependent variables
    attr_accessor :sx_P # pressure (dynes/cm^2)
    attr_accessor :sx_RHO # density (gm/cm^3)
    attr_accessor :sx_OPACITY # opacity
    attr_accessor :sx_U # specific energy (ergs/g)
    attr_accessor :sx_S # specific entropy (ergs/K/g)
    attr_accessor :sx_MU # grams per mole of gas particles (including electrons) -- mean particle molecular weight
    attr_accessor :sx_NE # free electron abundance (moles per gram of star) -- 1/NE is mean molecular weight per free electron
    attr_accessor :sx_NE_TOT # total electron abundance (moles per gram of star)
    attr_accessor :sx_SCP # specific heat capacity at constant pressure (ergs per gram per K)
    attr_accessor :sx_WF # weight factor for converting ergs/g/sec to total power for this shell (in Lsolar units)
       # incorporates delta_M plus conversion to solar units

    # the pressure is sum of the following terms
    attr_accessor :sx_PRAD # radiation pressure
    attr_accessor :sx_PEL # electron gas pressure
    attr_accessor :sx_PION # ion gas pressure
    attr_accessor :sx_PCORR # correction term for non-ideal gas effects
 
    # variables related to temperature gradients and convection
    attr_accessor :sx_GRAD_AD # adiabatic temperature gradient
    attr_accessor :sx_GRAD_RAD # radiative temperature gradient required to carry entire luminosity without convection
    attr_accessor :sx_GRAD_STAR # actual temperature gradient
    attr_accessor :sx_SG # diffusion coefficient for convective mixing (if is 0, then no mixing)
    attr_accessor :sx_CV # convection velocity -- zero if not in a convection zone
    attr_accessor :sx_WL # convection velocity times mixing length

    # nuclear power currently being generated everywhere inside this meshpoint (in Lsolar units)
    attr_accessor :sx_L_H # from hydrogen burning
    attr_accessor :sx_L_HE # from helium burning
    attr_accessor :sx_L_Z # from metals burning
    attr_accessor :sx_L_NEU # lost to neutrinos
     
    # categories of local power sources at this shell (ergs/g/sec)
    attr_accessor :sx_EPS_NUC # nuclear energy generation rate
    attr_accessor :sx_EPS_NEU # neutrino energy loss rate

    attr_accessor :sx_DQ_DK
        # change in mesh spacing function Q at this shell -- i.e., approximation for dQ/dk where k is the shell index.
        # this should be the same for all the shells when the spacing is optimal

    # variables related to the primary hydrogen burning reactions, PP chain and CNO cycle (ergs/g/sec)
    attr_accessor :sx_EPS_H # total energy generation rate from hydrogen burning by PP and CNO at this location 
    attr_accessor :sx_EPS_PP # PP chain energy generation rate
    attr_accessor :sx_EPS_CNO # CNO cycle energy generation rate

    # variables related to helium burning reactions (ergs/g/sec)
    attr_accessor :sx_EPS_HE # total energy generation rate from helium burning at this location
      
    # helium burning is the sum of the following reactions (ergs/g/sec)
    attr_accessor :sx_EPS_3A # triple alpha
    attr_accessor :sx_EPS_AC # C12 + He4 -> O16
    attr_accessor :sx_EPS_AN # N14 + 3/2 He4 -> Ne20
    attr_accessor :sx_EPS_AO # O16 + He4 -> Ne20
    attr_accessor :sx_EPS_ANE # Ne20 + He4 -> Mg24
          
   # variables related to other nuclear reactions -- metal burning that doesn't consume helium (ergs/g/sec)
    attr_accessor :sx_EPS_Z # total energy generation rate from metal burning at this location

    # metal burning is the sum of the following reactions (ergs/g/sec)
    attr_accessor :sx_EPS_CCA # 2 C12 -> Ne20 + He4
    attr_accessor :sx_EPS_CO # C12 + O16 -> Mg24 + He4
    attr_accessor :sx_EPS_OO # 2 O16 -> Mg24 + 2 He4
    attr_accessor :sx_EPS_GNE # Ne20 -> O16 + He4
    attr_accessor :sx_EPS_CCG # 2 C12 -> Mg24
    attr_accessor :sx_EPS_GMG # Mg24 -> Ne20 + He4

    # breakdown of neutrino losses (ergs/g/sec) -- only computed if ITOH_VINTAGE is at least 1996
    attr_accessor :sx_NEU_plasma # from plasmon neutrinos 
    attr_accessor :sx_NEU_brem # from bremsstrahlung
    attr_accessor :sx_NEU_pair # from pair annihilation
    attr_accessor :sx_NEU_photo # from photo neutrinos

    # various logarithmic derivatives
    attr_accessor :sx_HI1 # actual dln(RHO)/dln(P)
    attr_accessor :sx_HI2 # actual -dln(R)/dln(P)
    attr_accessor :sx_HI3 # actual -dln(M)/dln(P)
    attr_accessor :sx_GAMMA1 # adiabatic exponent, dln(P)/dln(RHO) at constant entropy.
      
    attr_accessor :sx_XH2 # mass fraction of molecular hydrogen
    attr_accessor :sx_XH_plus # neutral H mass fraction = SX_XH - SX_XH_plus - SX_XH2
    attr_accessor :sx_XHE_plus1 # neutral HE mass fraction = SX_XHE - SX_XHE_plus1 - SX_XHE_plus2
    attr_accessor :sx_XHE_plus2
    attr_accessor :sx_GAM
    
    @@num_sx_lines = 68
    @@num_lines = @@num_header_lines + @@num_sx_lines
    
    attr_accessor :sx_logT, :sx_logRHO, :sx_logP, :sx_logR
    attr_accessor :sx_logXH, :sx_logXHE, :sx_logXC, :sx_logXN, :sx_logXO, :sx_logXNE
    attr_accessor :sx_XH0, :sx_XHE0, :sx_CS, :sx_dlnp_dlnm
    attr_accessor :opacity_max, :entropy_max, :luminosity_max, :energy_max, :density_max
    attr_accessor :temp_max, :star_Age_String, :initial_Z_String, :initial_Mass_String
    attr_accessor :shell_with_max_temp, :first_shell, :last_shell, :surface_shell, :center_shell
    
    def initialize
        @dest = Array.new(@@num_lines) { Dvector.new }
        @sx_logT = Dvector.new
        @sx_logRHO = Dvector.new
        @sx_logP = Dvector.new
        @sx_logR = Dvector.new
        
        @sx_logXH = Dvector.new
        @sx_logXHE = Dvector.new
        @sx_logXC = Dvector.new
        @sx_logXN = Dvector.new
        @sx_logXO = Dvector.new
        @sx_logXNE = Dvector.new
        
        @sx_CS = Dvector.new
        
        @sx_XH0 = Dvector.new
        @sx_XHE0 = Dvector.new
        
        @sx_dlnp_dlnm = Dvector.new
        
        return self
    end
    
    def unpack_rows(filename)
        sizes = [3, 5, 4, 4, 3, 4, 4, 5, 3, 3, 4, 7, 6, 4]
        sizes.each_index do |i|
            sz = sizes[i]
            if @dest[i].size != sz
                puts "Unable to read profile from #{filename} row #{i} length #{@dest[i].size} expected #{sz}"
                raise "Unable to read profile from #{filename} row #{i} length #{@dest[i].size} expected #{sz}"
            end
        end
        sx = @dest[@@num_header_lines..-1]
        if (sx.size != @@num_sx_lines)
            puts "failed to find expected number of lines: found #{sx.size}, expected #{@@num_sx_lines}"
            raise "failed to find expected number of lines: found #{sx.size}, expected #{@@num_sx_lines}"
        end
        sx.each do |ary|
            if (ary.size != @@num_shells)
                put "Unable to read profile from #{filename} ary.size #{ary.size}"
                raise "Unable to read profile from #{filename}"
            end
        end
        row = @dest[0]
        @model_Number = row[0]; @star_Age = row[1]; @time_Step = row[2]
        row = @dest[1]
        @log_Luminosity = row[0]; @log_Radius = row[1]; @log_surface_Temp = row[2];
            @log_center_Temp = row[3]; @log_center_Density = row[4]
        row = @dest[2]
        @log_center_Pressure = row[0]; @center_Degeneracy = row[1]; @center_H = row[2]; @center_He = row[3]
        row = @dest[3]
        @center_C = row[0]; @center_N = row[1]; @center_O = row[2]; @center_Ne = row[3]
        row = @dest[4]
        @center_Mg = row[0]; @center_Si = row[1]; @center_Fe = row[2]
        row = @dest[5]
        @initial_Mass = row[0]; @star_Mass = row[1]; @star_Mdot = row[2]; @initial_Z = row[3]
        row = @dest[6]
        @star_Mass_H = row[0]; @star_Mass_He = row[1]; @star_Mass_C = row[2]; @star_Mass_N = row[3]
        row = @dest[7]
        @star_Mass_O = row[0]; @star_Mass_Ne = row[1]; @star_Mass_Mg = row[2]; @star_Mass_Si = row[3]; @star_Mass_Fe = row[4]
        row = @dest[8]
        @mass_He_Core = row[0]; @mass_C_Core = row[1]; @mass_O_Core = row[2]
        row = @dest[9]
        @dynamic_Timescale = row[0]; @thermal_Timescale = row[1]; @nuclear_Timescale = row[2]
        row = @dest[10]
        @power_H_burn = row[0]; @power_He_burn = row[1]; @power_Metal_burn = row[2]; @power_Neutrinos = row[3]
        row = @dest[11]
        @power_PP = row[0]; @power_CNO = row[1]; @power_3_alpha = row[2]; @power_C_alpha = row[3];
            @power_N_alpha = row[4]; @power_O_alpha = row[5]; @power_Ne_alpha = row[6]
        row = @dest[12]
        @power_CC_Ne = row[0]; @power_CO = row[1]; @power_OO = row[2]; @power_Ne_decay = row[3];
            @power_Mg_decay = row[4]; @power_CC_Mg = row[5]
        row = @dest[13]
        @power_plasmon_neutrinos = row[0]; @power_brem_neutrinos = row[1];
            @power_pair_neutrinos = row[2]; @power_photo_neutrinos = row[3]
        
        # to check the data file, sx[i] is on line i+@@num_header_lines = (i+15)
        @sx_R = sx[0] # radius (Rsolar)
        @sx_T = sx[1] # temperature (K)
        @sx_M = sx[2] # mass interior to this meshpoint (Msolar)
        @sx_L = sx[3]
        # luminosity (Lsolar) -- net rate of outward energy flow at this meshpoint (nuclear plus thermal minus neutrino)
        # this can be negative as a result of neutrino cooling at the center
        @sx_PSI = sx[4] # the degeneracy parameter (psi is the same as defined in Kippenhahn & Weigert)
        # psi replaces density or pressure as an independent variable

    # composition variables -- all are local mass fractions
        @sx_XH = sx[5] # H1
        @sx_XHE = sx[6] # He4
        @sx_XC = sx[7] # C12
        @sx_XN = sx[8] # N14
        @sx_XO = sx[9] # O16
        @sx_XNE = sx[10] # Ne20
        @sx_XMG = sx[11] # Mg24

    # some of the major dependent variables
        @sx_P = sx[12] # pressure (dynes/cm^2)
        @sx_RHO = sx[13] # density (gm/cm^3)
        @sx_OPACITY = sx[14] # opacity
        @sx_U = sx[15] # specific energy (ergs/g)
        @sx_S = sx[16] # specific entropy (ergs/K/g)
        @sx_MU = sx[17] # grams per mole of gas particles (including electrons) -- mean particle molecular weight
        @sx_NE = sx[18] # free electron abundance (moles per gram of star) -- 1/NE is mean molecular weight per free electron
        @sx_NE_TOT = sx[19] # total electron abundance (moles per gram of star)
        @sx_SCP = sx[20] # specific heat capacity at constant pressure (ergs per gram per K)
        @sx_WF = sx[21] # weight factor for converting ergs/g/sec to total power for this shell (in Lsolar units)
       # incorporates delta_M plus conversion to solar units

    # the pressure is sum of the following terms
        @sx_PRAD = sx[22] # radiation pressure
        @sx_PEL = sx[23] # electron gas pressure
        @sx_PION = sx[24] # ion gas pressure
        @sx_PCORR = sx[25] # correction term for non-ideal gas effects
 
    # variables related to temperature gradients and convection
        @sx_GRAD_AD = sx[26] # adiabatic temperature gradient
        @sx_GRAD_RAD = sx[27] # radiative temperature gradient required to carry entire luminosity without convection
        @sx_GRAD_STAR = sx[28] # actual temperature gradient
        @sx_SG = sx[29] # diffusion coefficient for convective mixing (if is 0, then no mixing)
        @sx_CV = sx[30] # convection velocity -- zero if not in a convection zone
        @sx_WL = sx[31] # convection velocity times mixing length

    # nuclear power currently being generated everywhere inside this meshpoint (in Lsolar units)
        @sx_L_H = sx[32] # from hydrogen burning
        @sx_L_HE = sx[33] # from helium burning
        @sx_L_Z = sx[34] # from metals burning
        @sx_L_NEU = sx[35] # lost to neutrinos
     
    # categories of local power sources at this shell (ergs/g/sec)
        @sx_EPS_NUC = sx[36] # nuclear energy generation rate
        @sx_EPS_NEU = sx[37] # neutrino energy loss rate

        @sx_DQ_DK = sx[38]
        # change in mesh spacing function Q at this shell -- i.e., approximation for dQ/dk where k is the shell index.
        # this should be the same for all the shells when the spacing is optimal

    # variables related to the primary hydrogen burning reactions, PP chain and CNO cycle (ergs/g/sec)
        @sx_EPS_H = sx[39] # total energy generation rate from hydrogen burning by PP and CNO at this location 
        @sx_EPS_PP = sx[40] # PP chain energy generation rate
        @sx_EPS_CNO = sx[41] # CNO cycle energy generation rate

    # variables related to helium burning reactions (ergs/g/sec)
        @sx_EPS_HE = sx[42] # total energy generation rate from helium burning at this location
      
    # helium burning is the sum of the following reactions (ergs/g/sec)
        @sx_EPS_3A = sx[43] # triple alpha
        @sx_EPS_AC = sx[44] # C12 + He4 -> O16
        @sx_EPS_AN = sx[45] # N14 + 3/2 He4 -> Ne20
        @sx_EPS_AO = sx[46] # O16 + He4 -> Ne20
        @sx_EPS_ANE = sx[47] # Ne20 + He4 -> Mg24
          
   # variables related to other nuclear reactions -- metal burning that doesn't consume helium (ergs/g/sec)
        @sx_EPS_Z = sx[48] # total energy generation rate from metal burning at this location

    # metal burning is the sum of the following reactions (ergs/g/sec)
        @sx_EPS_CCA = sx[49] # 2 C12 -> Ne20 + He4
        @sx_EPS_CO = sx[50] # C12 + O16 -> Mg24 + He4
        @sx_EPS_OO = sx[51] # 2 O16 -> Mg24 + 2 He4
        @sx_EPS_GNE = sx[52] # Ne20 -> O16 + He4
        @sx_EPS_CCG = sx[53] # 2 C12 -> Mg24
        @sx_EPS_GMG = sx[54] # Mg24 -> Ne20 + He4

    # breakdown of neutrino losses (ergs/g/sec) -- only computed if ITOH_VINTAGE is at least 1996
        @sx_NEU_plasma = sx[55] # from plasmon neutrinos 
        @sx_NEU_brem = sx[56] # from bremsstrahlung
        @sx_NEU_pair = sx[57] # from pair annihilation
        @sx_NEU_photo = sx[58] # from photo neutrinos

    # various logarithmic derivatives
        @sx_HI1 = sx[59] # actual dln(RHO)/dln(P)
        @sx_HI2 = sx[60] # actual -dln(R)/dln(P)
        @sx_HI3 = sx[61] # actual -dln(M)/dln(P)
        @sx_GAMMA1 = sx[62] # adiabatic exponent, dln(P)/dln(RHO) at constant entropy.
      
    # mass fractions for various ionization states of H, He, C, N, and O
        @sx_XH2 = sx[63] # mass fraction of molecular hydrogen
        @sx_XH_plus = sx[64] # neutral H mass fraction = SX_XH - SX_XH_plus - SX_XH2
        @sx_XHE_plus1 = sx[65] # neutral HE mass fraction = SX_XHE - SX_XHE_plus1 - SX_XHE_plus2
        @sx_XHE_plus2 = sx[66]
        
        @sx_GAM = sx[67]
        
        @sx_logT.replace(@sx_T).safe_log10!
        @sx_logRHO.replace(@sx_RHO).safe_log10!
        @sx_logP.replace(@sx_P).safe_log10!
        @sx_logR.replace(@sx_R).safe_log10!
        
        @sx_logXH = @sx_XH.safe_log10
        @sx_logXHE.replace(@sx_XHE).safe_log10!
        @sx_logXC.replace(@sx_XC).safe_log10!
        @sx_logXN.replace(@sx_XN).safe_log10!
        @sx_logXO.replace(@sx_XO).safe_log10!
        @sx_logXNE.replace(@sx_XNE).safe_log10!
        
        @sx_XH0.replace(@sx_XH).sub!(@sx_XH_plus).sub!(@sx_XH2)
        @sx_XHE0.replace(@sx_XHE).sub!(@sx_XHE_plus1).sub!(@sx_XHE_plus2)
        
        msolar = 1.9891e33
        rsolar = 0.69598e11
        @sx_dlnp_dlnm.replace(@sx_M).mul!(msolar)
        @sx_dlnp_dlnm.mul!(@sx_dlnp_dlnm).mul!(-6.672e-8)
        @sx_dlnp_dlnm.div!(4 * 3.14159265358979).div!(@sx_P)
        @sx_dlnp_dlnm.div!(@sx_R).div!(@sx_R).div!(@sx_R).div!(@sx_R)
        @sx_dlnp_dlnm.div!(rsolar**4)
        
        @sx_CS = @sx_P / @sx_RHO
        @sx_CS.safe_sqrt!

        @shell_with_max_temp = @sx_T.where_max
        @temp_max = @sx_T[@shell_with_max_temp]
        @opacity_max = @sx_OPACITY.max
        @density_max = @sx_RHO.max
        @entropy_max = @sx_S.max
        @luminosity_max = @sx_L.max
        @energy_max = @sx_U.max
        
        @first_shell = 0
        @surface_shell = @sx_R.where_max
        @last_shell = @sx_R.size - 1
        @center_shell = @sx_P.where_max    
    
        if @star_Age > 1e9
            @star_Age_String = sprintf("%0.3f (Gyr)", @star_Age * 1e-9)
        else
            @star_Age_String = sprintf("%0.3f (Myr)", @star_Age * 1e-6)
        end
        
        if @initial_Mass >= 10 and @initial_Mass == @initial_Mass.floor
            @initial_Mass_String = sprintf('$M_{init}$=%0.0f', @initial_Mass)
        else
            @initial_Mass_String = sprintf('$M_{init}$=%0.1f', @initial_Mass)
        end
        
        if @initial_Z >= 0.01
            @initial_Z_String = sprintf('$Z_{init}$=%0.2f', @initial_Z)
        elsif @initial_Z >= 0.001
            @initial_Z_String = sprintf('$Z_{init}$=%0.3f', @initial_Z)
        else
            @initial_Z_String = sprintf('$Z_{init}$=%0.4f', @initial_Z)
        end
        
    end
    
    def save_stuff
        puts "write_C_O_profile"
        file = File.open('C_O_profile.data', 'w')
        file.printf(
            'Columns contain: Mass XC XO XNE XN.  ' +
            'Initial mass = %0.2g  Current mass = %0.4g   ' +
            'Age = %0.4g   Initial Z = %g' + "\n",
            @initial_Mass, @star_Mass, @star_Age, @initial_Z)
        @sx_M.each_index do |i|
            if @sx_XC[i] > @sx_XH[i]
                file.printf("%12.5g %12.5g %12.5g %12.5g %12.5g\n",
                    @sx_M[i], @sx_XC[i], @sx_XO[i],
                    @sx_XNE[i], @sx_XN[i])
            end
        end
        file.close
    end
    
    def read_profile(filename)
        saving_stuff = false # change to true when saving
        keep_trying = 5
        while keep_trying > 0
            begin
                Dvector.read_rows(filename, @dest)
                unpack_rows(filename)
                keep_trying = 0
                save_stuff if saving_stuff
            rescue
                if keep_trying == 0
                    raise "Unable to read profile from #{filename}"
                end
                sleep 0.5
            end
            keep_trying -= 1
        end
    end

end
