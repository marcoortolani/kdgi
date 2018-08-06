from gi_gipy import *
import os
import xlsxwriter
from xlsxwriter.utility import xl_range

# range(min_included, max_NOT_included, incremento)
for n_symbols in range(6, 8):

    print('\nProcessing n_symbols = ' + str(n_symbols) + '\n\n')

    dir_path = './new_struct_sim_experiments/' + str(n_symbols) + '_symbols/results'
    if not os.path.exists(dir_path):
        os.makedirs(dir_path)

    # Create a workbook and add a worksheet.
    xls_path = dir_path + '/intra_n_' + str(n_symbols) + '_symbols_results.xlsx'
    workbook = xlsxwriter.Workbook(xls_path)
    worksheet = workbook.add_worksheet()

    # Excel formatting:
    # Start from the first cell. Rows and columns are zero indexed.
    row = 0
    col = 0
    worksheet.write(row, col, 'Confronti tra automi di ' + str(n_symbols) + ' simboli, per ciascun livello n sono generati casualmente 10 automi, ciascuno di essi verra confrontato con ogni altro del suo livello per un totale di 45 confronti, dato che n varia da 2 a 50')
    
    row += 2

    row_final_results = 54
    col_final_results = 0
    worksheet.write(row_final_results, col_final_results, 'N')
    worksheet.write(row_final_results, col_final_results + 1, 'Average')
    worksheet.write(row_final_results, col_final_results + 2, 'Variance')
    worksheet.write(row_final_results, col_final_results + 3, 'Iterazioni')
    row_final_results += 1

    for n in range(2, 51):

        print('Processing Abbadingo parameter N = ' + str(n) + '\n')

        # Excel formatting:
        # Start from the first cell. Rows and columns are zero indexed.
        worksheet.write(row, col, 'N = ' + str(n))
        worksheet.write(row, col + 2, 'Similarita')
        worksheet.write(row, col + 3, 'Iterazioni')

        row += 2
       

        for automata_number in range(1, 10):

            dfa_name1 = 'n' + str(n) + '_' + str(automata_number)

            dfa_path1 = './new_struct_sim_experiments/' + str(n_symbols) + '_symbols/automata/n' + str(n) + '/' + dfa_name1 + '.txt'

            dfa1 = Dfa.read_dfa_file(dfa_path1)

            for automata_number2 in range(automata_number+1, 11):

                dfa_name2 = 'n' + str(n) + '_' + str(automata_number2)

                dfa_path2 = './new_struct_sim_experiments/' + str(n_symbols) + '_symbols/automata/n' + str(n) + '/' + dfa_name2 + '.txt'

                dfa2 = Dfa.read_dfa_file(dfa_path2)

                sim_v = dfa1.struct_sim(dfa2)

                sim = sim_v[0]

                iterazioni = sim_v[1]

                description = dfa_name1 + ' <-> ' + dfa_name2

                worksheet.write(row, col, description)
                worksheet.write(row, col + 2, sim)
                worksheet.write(row, col + 3, iterazioni)
                row += 1

        row += 1

        worksheet.write(row, col, 'Average')
        worksheet.write(row + 1, col, 'Variance')
        worksheet.write(row + 2, col, 'Iterazioni')

        cell_range = xl_range(4, col+2, 48, col+2)
        cell_range2 = xl_range(4, col+3, 48, col+3)

        formula = '=AVERAGE(%s)' % cell_range
        formula2 = '=VAR(%s)' % cell_range
        formula3 = '=AVERAGE(%s)' % cell_range2
        worksheet.write(row, col + 2, formula)
        worksheet.write(row + 1, col + 2, formula2)
        worksheet.write(row + 2, col + 2, formula3)

        worksheet.write(row_final_results, col_final_results, n)
        worksheet.write(row_final_results, col_final_results + 1, formula)
        worksheet.write(row_final_results, col_final_results + 2, formula2)
        worksheet.write(row_final_results, col_final_results + 3, formula3)


        # I risultati di n_mobile successivo riportali a destra con uno spazio in mezzo
        # la riga torna a essere la terza
        col += 6
        row = 2
        row_final_results += 1

    workbook.close()





