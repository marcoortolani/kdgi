from gi_gipy import *
import os
import xlsxwriter
from xlsxwriter.utility import xl_range

# range(min_included, max_NOT_included, incremento)
for n_symbols in range(6, 7):

    # print('\nProcessing n_symbols = ' + str(n_symbols) + '\n\n')

    inter_n_path = './struct_sim_experiments/' + str(n_symbols) + '_symbols/results/inter_n'
    if not os.path.exists(inter_n_path):
                os.makedirs(inter_n_path)

    
    for n_fisso in range(5, 6):

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
        worksheet.write(row, col, 'Confronti tra n_fisso = ' + str(n_fisso) + ' e n variabile da 2 a 30')
        # worksheet.write(row, col + 3, 'Similarity score')

        row += 2

        row_final_results = 108
        col_final_results = 0
        worksheet.write(row_final_results, col_final_results, 'N_mobile')
        worksheet.write(row_final_results, col_final_results + 1, 'Average')
        worksheet.write(row_final_results, col_final_results + 2, 'Variance')
        row_final_results += 1

        # ogni automa del livello n fisso, deve essere confrontato con ogni automa dei livelli da 2 a 30
        # producendo 100 confronti per ogni n mobile
        for n_mobile in range(2,31):

            print('Processing N_mobile = ' + str(n_mobile) + '\n')

            worksheet.write(row, col, 'N_mobile = ' + str(n_mobile))
            row += 2

            for automata_number in range(1, 11):

                dfa_name1 = 'n' + str(n_fisso) + '_' + str(automata_number)

                dfa_path1 = './struct_sim_experiments/' + str(n_symbols) + '_symbols/automata/n' + str(n_fisso) + '/' + dfa_name1 + '.txt'

                dfa1 = Dfa.read_dfa_file(dfa_path1) 

                for automata_number2 in range(1, 11):

                    dfa_name2 = 'n' + str(n_mobile) + '_' + str(automata_number2)

                    dfa_path2 = './struct_sim_experiments/' + str(n_symbols) + '_symbols/automata/n' + str(n_mobile) + '/' + dfa_name2 + '.txt'

                    dfa2 = Dfa.read_dfa_file(dfa_path2)

                    sim_v = dfa1.struct_sim(dfa2, 0.000001)

                    sim = sim_v[0]

                    description = dfa_name1 + ' <-> ' + dfa_name2

                    worksheet.write(row, col, description)
                    worksheet.write(row, col + 2, sim)
                    row += 1

            row += 1

            # Write the medium similarity between automatas of level n using a formula.
            worksheet.write(row, col, 'Average')
            worksheet.write(row + 1, col, 'Variance')
            # Se confronto i 10 automi di un n fisso, con tutti i 10 automi di un n mobile
            # ho in totale 100 confronti per ogni n mobile

            cell_range = xl_range(4, col+2, 103, col+2)

            formula = '=AVERAGE(%s)' % cell_range
            formula2 = '=VAR(%s)' % cell_range
            worksheet.write(row, col + 2, formula)
            worksheet.write(row + 1, col + 2, formula2)
            worksheet.write(row_final_results, col_final_results, str(n_mobile))
            #worksheet.write(row_final_results, col_final_results + 1, worksheet.cell_value(row, col + 2))
            worksheet.write(row_final_results, col_final_results + 1, formula)
            worksheet.write(row_final_results, col_final_results + 2, formula2)


            # I risultati di n_mobile successivo riportali a destra con uno spazio in mezzo
            # la riga torna a essere la terza
            col += 4
            row = 2
            row_final_results += 1
            
            # worksheet.write(row+1, 0, 'n')
            # worksheet.write(row+1, 3, str(n))

        # finito un n_fisso per tutti gli n_mobili posso chiudere il file con i risultati
        workbook.close()





