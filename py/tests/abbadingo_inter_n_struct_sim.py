from gi_gipy import *
import os
import xlsxwriter
from xlsxwriter.utility import xl_range

# range(min_included, max_NOT_included, incremento)
for n_symbols in range(2, 9):

    # print('\nProcessing n_symbols = ' + str(n_symbols) + '\n\n')

    new_exp_path = './new_struct_sim_experiments'
    if not os.path.exists(new_exp_path):
        os.makedirs(new_exp_path)

    n_symbols_path = new_exp_path + '/' + str(n_symbols) + '_symbols'
    if not os.path.exists(n_symbols_path):
        os.makedirs(n_symbols_path)

    results_path = n_symbols_path + '/results'
    if not os.path.exists(results_path):
        os.makedirs(results_path)

    inter_n_path = results_path + '/inter_n'
    if not os.path.exists(inter_n_path):
        os.makedirs(inter_n_path)

    
    for n_fisso in range(10, 31, 5):

        print('Processing N_fisso = ' + str(n_fisso) + ' with \n')
        
        dir_path = inter_n_path + '/n_fisso_' + str(n_fisso)
        if not os.path.exists(dir_path):
            os.makedirs(dir_path)

        # Crea il file excel per un n_fisso con tutti gli n_mobili
        xls_path = dir_path + '/results_' + str(n_symbols) + '_symbols_n_' + str(n_fisso) + '.xlsx'
        workbook = xlsxwriter.Workbook(xls_path)
        worksheet = workbook.add_worksheet()

        # Excel formatting:
        # Start from the first cell. Rows and columns are zero indexed.
        row = 0
        col = 0
        worksheet.write(row, col, 'Confronti tra n_fisso = ' + str(n_fisso) + ' e n variabile da 2 a 50')
        # worksheet.write(row, col + 3, 'Similarity score')

        row += 2

        row_final_results = 109
        col_final_results = 0
        worksheet.write(row_final_results, col_final_results, 'N_mobile')
        worksheet.write(row_final_results, col_final_results + 1, 'Average')
        worksheet.write(row_final_results, col_final_results + 2, 'Variance')
        worksheet.write(row_final_results, col_final_results + 3, 'Iterazioni')
        row_final_results += 1

        # ogni automa del livello n fisso, deve essere confrontato con ogni automa dei livelli da 2 a 30
        # producendo 100 confronti per ogni n mobile
        for n_mobile in range(2,51):

            print('Processing N_mobile = ' + str(n_mobile) + '\n')

            worksheet.write(row, col, 'N_mobile = ' + str(n_mobile))
            worksheet.write(row, col + 2, 'Similarita')
            worksheet.write(row, col + 3, 'Iterazioni')
            row += 2

            for automata_number in range(1, 11):

                dfa_name1 = 'n' + str(n_fisso) + '_' + str(automata_number)

                dfa_path1 = n_symbols_path + '/automata/n' + str(n_fisso) + '/' + dfa_name1 + '.txt'

                dfa1 = Dfa.read_dfa_file(dfa_path1) 

                for automata_number2 in range(1, 11):

                    dfa_name2 = 'n' + str(n_mobile) + '_' + str(automata_number2)

                    dfa_path2 = n_symbols_path + '/automata/n' + str(n_mobile) + '/' + dfa_name2 + '.txt'

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

            # Write the medium similarity between automatas of level n using a formula.
            worksheet.write(row, col, 'Average')
            worksheet.write(row + 1, col, 'Variance')
            worksheet.write(row + 2, col, 'Iterazioni')
            # Se confronto i 10 automi di un n fisso, con tutti i 10 automi di un n mobile
            # ho in totale 100 confronti per ogni n mobile

            cell_range = xl_range(4, col+2, 103, col+2)
            cell_range2 = xl_range(4, col+3, 103, col+3)

            formula = '=AVERAGE(%s)' % cell_range
            formula2 = '=VAR(%s)' % cell_range
            formula3 = '=AVERAGE(%s)' % cell_range2
            worksheet.write(row, col + 2, formula)
            worksheet.write(row + 1, col + 2, formula2)
            worksheet.write(row + 2, col + 2, formula3)
            worksheet.write(row_final_results, col_final_results, n_mobile)
            #worksheet.write(row_final_results, col_final_results + 1, worksheet.cell_value(row, col + 2))
            worksheet.write(row_final_results, col_final_results + 1, formula)
            worksheet.write(row_final_results, col_final_results + 2, formula2)
            worksheet.write(row_final_results, col_final_results + 3, formula3)


            # I risultati di n_mobile successivo riportali a destra con uno spazio in mezzo
            # la riga torna a essere la terza
            col += 6
            row = 2
            row_final_results += 1
            
            # worksheet.write(row+1, 0, 'n')
            # worksheet.write(row+1, 3, str(n))

        # finito un n_fisso per tutti gli n_mobili posso chiudere il file con i risultati
        workbook.close()





