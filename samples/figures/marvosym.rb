#  marvosym.rb

# "Marvosym" is a free symbols font designed by Martin Vogel (http://www.marvosym.com/)
# It may already be a part of your TeX distribution 
#       (it is included with Gerben Wierda's i-Installer http://ii2.sourceforge.net/tex-index.html)
# To use it, add \usepackage{marvosym} to the TeX preamble (see initialize below)
# To add a symbol, use one of the defined macros such as \Smiley as part of a text string.
# The symbol will appear in the currently selected font size, so you can get a bigger symbol,
# by using a size-changing command like {\Large \Smiley}.


class Marvosym

    include Tioga
    include FigureConstants
    
    def t
        @figure_maker
    end

    def initialize
        @figure_maker = FigureMaker.default
        t.save_dir = 'figures_out'
        t.tex_preview_preamble = t.tex_preview_preamble + "\n\t\\usepackage{marvosym}\n"
        t.def_figure("sample") { sample }
        t.def_figure("communication") { communication }
        t.def_figure("engineering") { engineering }
        t.def_figure("information") { information }
        t.def_figure("currency") { currency }
        t.def_figure("safety") { safety }
        t.def_figure("navigation") { navigation }
        t.def_figure("computers") { computers }
        t.def_figure("maths") { maths }
        t.def_figure("biology") { biology }
        t.def_figure("astronomy") { astronomy }
        t.def_figure("astrology") { astrology }
        t.def_figure("others") { others }
    end
        
    def background
        clr = t.rgb_to_hls(Tan)
        clr[1] = 0.97
        t.fill_color = t.hls_to_rgb(clr)
        t.line_width = 2
        t.fill_and_stroke_frame
    end
        
    def show_marvosym(name, x, y)
        x_name = x - 0.03
        x_marker = x + 0.03
        dy = - t.default_text_height_dy * 0.2
        t.show_text('text' => name, 'x' => x_name, 'y' => y, 'justification' => RIGHT_JUSTIFIED)
        t.show_text('text' => '\\' + name, 'x' => x_marker, 'y' => y+dy, 'scale' => 1.8, 'justification' => CENTERED);
    end
    
    def threecolumns(list1, list2, list3)
        t.set_landscape
        background
        t.rescale(0.5)
        dy = -t.default_text_height_dy * 2.4
        t.fill_color = Black
        x = 0.24; y = y_init = 0.87
        list1.each { |name| show_marvosym(name, x, y); y += dy }
        x = 0.55; y = y_init
        list2.each { |name| show_marvosym(name, x, y); y += dy }
        x = 0.89; y = y_init
        list3.each { |name| show_marvosym(name, x, y); y += dy }
    end

    def sample
        list1 = [ 'EUR', 'Radioactivity', 'Football']
        list2 = [ 'Smiley', 'Frowny', 'Yinyang' ]
        list3 = [ 'Bat', 'Male', 'Female' ]
        threecolumns(list1, list2, list3)
    end
    
    def communication
        list1 = [ 'Pickup', 'Letter', 'Mobilefone', 'Telefon']
        list2 = [ 'fax', 'FAX', 'Faxmachine' ]
        list3 = [ 'Email', 'Lightning', 'Emailct' ]
        threecolumns(list1, list2, list3)
    end
    
    def engineering
        list1 = [ 'Beam', 'Bearing', 'Loosebearing', 'Fixedbearing', 'Lefttorque', 'Righttorque', 'Lineload', 'Force']
        list2 = [ 'Octosteel', 'Hexasteel', 'Squaresteel', 'Rectsteel', 'Circsteel', 'Flatsteel', 'Squarepipe', 'Rectpipe']
        list3 = [ 'Lsteel', 'TTsteel', 'Circpipe', 'Tsteel', 'RoundedTsteel', 'RoundedTTsteel', 'RoundedLsteel']
        threecolumns(list1, list2, list3)
    end
    
    def information
        list1 = [ 'Industry', 'Coffeecup', 'Rightscissors', 'Leftscissors', 'Football', 'Bicycle']
        list2 = [ 'Info', 'Clocklogo', 'Cutright', 'Cutleft', 'Cutline', 'Wheelchair']
        list3 = [ 'Checkedbox', 'Crossedbox', 'Pointinghand', 'Writinghand', 'Gentsroom', 'Ladiesroom']
        threecolumns(list1, list2, list3)
    end
    
    def currency
        list1 = [ 'EUR', 'EURdig', 'EURhv', 'EURcr', 'EURtm']
        list2 = [ 'Ecommerce', 'Shilling', 'Denarius']
        list3 = [ 'Pfund', 'EyesDollar']
        threecolumns(list1, list2, list3)
    end
    
    def safety
        list1 = [ 'Stopsign', 'CEsign', 'Estatically']
        list2 = [ 'Explosionsafe', 'Laserbeam']
        list3 = [ 'Biohazard', 'Radioactivity', 'BSEfree']
        threecolumns(list1, list2, list3)
    end
    
    def navigation
        list1 = [ 'RewindToIndex', 'RewindToStart', 'Rewind']
        list2 = [ 'ForwardToIndex', 'ForwardToEnd', 'Forward']
        list3 = [ 'MoveUp', 'MoveDown', 'ToTop', 'ToBottom']
        threecolumns(list1, list2, list3)
    end
    
    def computers
        list1 = [ 'ComputerMouse', 'SerialInterface']
        list2 = [ 'Keyboard', 'Printer']
        list3 = [ 'SerialPort', 'ParallelPort']
        threecolumns(list1, list2, list3)
    end
    
    def maths
        list1 = [ 'MVZero', 'MVOne', 'MVTwo', 'MVThree', 'MVFour']
        list2 = [ 'MVFive', 'MVSix', 'MVSeven', 'MVEight', 'MVNine']
        list3 = [ 'Vectorarrowhigh', 'Vectorarrow', 'Corresponds', 'Anglesign', 'Squaredot']
        threecolumns(list1, list2, list3)
    end
    
    def biology
        list1 = [ 'Male', 'Female']
        list2 = [ 'Neutral', 'Hermaphrodite']
        list3 = [ 'MaleMale', 'FemaleFemale', 'FemaleMale']
        threecolumns(list1, list2, list3)
    end
    
    def astronomy
        list1 = [ 'Sun', 'Moon', 'Mercury', 'Venus']
        list2 = [ 'Earth', 'Mars', 'Jupiter', 'Saturn']
        list3 = [ 'Uranus', 'Neptune', 'Pluto']
        threecolumns(list1, list2, list3)
    end
    
    def astrology
        list1 = [ 'Aries', 'Taurus', 'Gemini', 'Cancer']
        list2 = [ 'Leo', 'Virgo', 'Libra', 'Scorpio']
        list3 = [ 'Sagittarius', 'Capricorn', 'Aquarius', 'Pisces']
        threecolumns(list1, list2, list3)
    end
    
    def others
        list1 = [ 'Yinyang', 'Rightarrow', 'MVAt', 'Mundus']
        list2 = [ 'Smiley', 'Frowny', 'Bat', 'Womanface', 'MartinVogel']
        list3 = [ 'Cross', 'Celtcross', 'Ankh', 'Heart']
        threecolumns(list1, list2, list3)
    end
    
end

Marvosym.new









