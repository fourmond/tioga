# plot.rb

# for use with IRB_Tioga

def plot(filename= 'result.txt', col = 2)
    load_figures('default_plot.rb')
    if filename.kind_of?Integer
        col = filename
        filename = 'result.txt'
    end
    $data = filename
    $y_column = col
    preview(col-2)
end
