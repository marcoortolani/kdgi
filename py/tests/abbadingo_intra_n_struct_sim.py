from gi_gipy import *
import xlsxwriter

for n in range(10, 11):

    # Create a workbook and add a worksheet.
    xls_path = './struct_sim_experiments/results/intra_n/n' + str(n) + '/results_n_' + str(n) + '.xlsx'
    workbook = xlsxwriter.Workbook(xls_path)
    worksheet = workbook.add_worksheet()

    # Excel formatting:
    # Start from the first cell. Rows and columns are zero indexed.
    row = 0
    col = 0
    worksheet.write(row, col, 'Comparisons intra level n = ' + str(n))
    worksheet.write(row, col + 3, 'Similarity score')
    #worksheet.write(row, col + 2, 'Iterations number')

    row += 2

    for automata_number in range(1, 3):

        dfa_name1 = 'n' + str(n) + '_' + str(automata_number)

        dfa_path1 = './struct_sim_experiments/automata/n' + str(n) + '/' + dfa_name1 + '.txt'

        dfa1 = Dfa.read_dfa_file(dfa_path1)

        for automata_number2 in range(automata_number+1, 4):

            dfa_name2 = 'n' + str(n) + '_' + str(automata_number2)

            dfa_path2 = './struct_sim_experiments/automata/n' + str(n) + '/' + dfa_name2 + '.txt'

            dfa2 = Dfa.read_dfa_file(dfa_path2)

            sim_v = dfa1.struct_sim(dfa2, 0.000001)

            sim = sim_v[0]

            n_iter = sim_v[1]

            description = dfa_name1 + ' <-> ' + dfa_name2

            worksheet.write(row, col, description)
            worksheet.write(row, col + 3, sim)
            #worksheet.write(row, col + 2, n_iter)
            row += 1

    row += 1
    # Write the medium similarity between automatas of level n using a formula.
    worksheet.write(row, 0, 'Average structural similarity between automatas created with parameter: '+str(n))
    worksheet.write(row, 3, '=AVERAGE(D3:D47)')
    #worksheet.write(row + 1, 0, 'Average number of iterations:')
    #worksheet.write(row + 1, 1, '=AVERAGE(C1:C45)')

    workbook.close()






