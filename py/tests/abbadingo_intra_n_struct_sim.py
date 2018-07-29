from gi_gipy import *
import os
import xlsxwriter

# range(min_included, max_NOT_included, incremento)
for n_symbols in range(3, 7):

    print('\nProcessing n_symbols = ' + str(n_symbols) + '\n\n')
    
    for n in range(10, 51, 10):

        print('Processing Abbadingo parameter N = ' + str(n) + '\n')
        
        dir_path = './struct_sim_experiments/' + str(n_symbols) + '_symbols/results/intra_n/n' + str(n)
        if not os.path.exists(dir_path):
                os.makedirs(dir_path)

        # Create a workbook and add a worksheet.
        xls_path = dir_path + '/results_n_' + str(n) + '.xlsx'
        workbook = xlsxwriter.Workbook(xls_path)
        worksheet = workbook.add_worksheet()

        # Excel formatting:
        # Start from the first cell. Rows and columns are zero indexed.
        row = 0
        col = 0
        worksheet.write(row, col, 'n = ' + str(n))
        worksheet.write(row, col + 3, 'Similarity score')

        row += 2

        for automata_number in range(1, 10):

            dfa_name1 = 'n' + str(n) + '_' + str(automata_number)

            dfa_path1 = './struct_sim_experiments/' + str(n_symbols) + '_symbols/automata/n' + str(n) + '/' + dfa_name1 + '.txt'

            dfa1 = Dfa.read_dfa_file(dfa_path1)

            for automata_number2 in range(automata_number+1, 11):

                dfa_name2 = 'n' + str(n) + '_' + str(automata_number2)

                dfa_path2 = './struct_sim_experiments/' + str(n_symbols) + '_symbols/automata/n' + str(n) + '/' + dfa_name2 + '.txt'

                dfa2 = Dfa.read_dfa_file(dfa_path2)

                sim_v = dfa1.struct_sim(dfa2, 0.000001)

                sim = sim_v[0]

                description = dfa_name1 + ' <-> ' + dfa_name2

                worksheet.write(row, col, description)
                worksheet.write(row, col + 3, sim)
                row += 1

        row += 1

        # Write the medium similarity between automatas of level n using a formula.
        worksheet.write(row, 0, 'Average '+str(n))
        worksheet.write(row, 3, '=AVERAGE(D3:D47)')
        worksheet.write(row+1, 0, 'n')
        worksheet.write(row+1, 3, str(n))

        workbook.close()





