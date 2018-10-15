from gi_gipy import *
import os
from pdf2image import convert_from_path

data_path = '../data'

dfa_name = 'palermo_ustica'

dfa_file = data_path + '/automata/' + '/thesis/hierarchical/' + dfa_name + '.txt'

x=Dfa(Dfa.read_dfa_file(dfa_file))

dot_path = data_path + '/automata_dots/' + dfa_name + '.dot'

x.print_dfa_dot(dfa_name, dot_path)

pics_path = data_path + '/automata_pics/' + dfa_name + '.pdf'

bashCommand = 'dot -Tpdf '+ dot_path + ' > ' + pics_path

#pages = convert_from_path(pics_path, 500)

#jpg_path = data_path + '/automata_pics/' + dfa_name + '.jpg'

#for page in pages:
#    page.save(jpg_path, 'JPEG')

os.system(bashCommand)