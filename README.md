# kdgi
Knowledge discovery through grammar inference

Nota: il codice include lo unit testing tramite Google test.
Le librerie di testing sono aggiunte come un submodule tramite il comando:

`git submodule add https://github.com/google/googletest.git gtest`

Per aggiornare il submodule, una volta clonato il progetto da git, si possono dare i comandi:

`git submodule init`

`git submodule update`

oppure direttamente:

`git clone --recursive https://github.com/marcoortolani/kdgi.git`

Per compilare:
`mkdir build cd build cmake -G "Unix Makefiles" ..`
`make`

oppure (per il testing e per il porting pybind):
`cmake -G "Unix Makefiles" -Dtest=on -Dpython=on ..`
`make`
`make test`

Se si vuole vedere quale specifico test è riuscito/fallito occorre eseguire, all'interno della cartella build:
`./runUnitTests`

Inoltre, il porting della libreria `gi` su python utilizza il progetto pybind11, aggiunto mediante il comando:

`git submodule add https://github.com/pybind/pybind11.git`

nella directory `py`

Per informazioni circa l'utilizzo di `git merge`  per condividere codice tra branches leggere:
git_merge.txt

Per generare la documentazione del codice, nella directory di compilazione (es. `build`):

`make docs`



-- ACTIVE LEARNING --
Per usare l'oracolo RNN sono necessari moduli python aggiuntivi (keras, pandas, sci-kit learn ...).
Il codice relativo all'oracolo RNN è contenuto nella cartella kdgi/rnnoracle, mentre esempi di codice che usano la libreria e l'oracolo RNN in python sono contenuti in  kdgi/py/copy_in_build_py.

Per usare l'oracolo RNN eseguire (dentro la cartella build):
`cmake -G "Unix Makefiles" -Dpyembed=on ..`

O per farne il porting in python:
`cmake -G "Unix Makefiles" -Dpyembed=on -Dpython=on ..
Nel qual caso la libreria e tutti i file .py necessari (kdgi/ker, kdgi/py/copy_in_build_py) saranno copiati in kdgi/build/py

Nel caso in cui si voglia indicare esplicitamente il percorso dell'interprete, si può fare con le opzioni  -DPYTHON_LIBRARY e -DPYTHON_INCLUDE_DIR (per esempio nel caso in cui un ambiente virtuale non funzioni come dovrebbe)
ad esempio:
cmake -G "Unix Makefiles" -Dpython=on -Dpyembed=on -DPYTHON_LIBRARY=/home/kdgi/anaconda3/envs/alearn/lib/libpython3.6m.so -DPYTHON_INCLUDE_DIR=/home/kdgi/anaconda3/envs/alearn/include/python3.6m ..

Dopo la compilazione:
Generare un dataset csv -> usare il dataset csv per addestrare una RNN -> usare la RNN per costruire l'oracolo
Esempi di dataset csv sono presenti in kdgi/ker/csv
Il codice in kdgi/build/py/gen_csv_data.py genera il file kdgi/build/py/csv/test.csv a partire dal  dfa prelevato da kdgi/build/py/data/_odd_a.txt
Il codice in kdgi/build/py/gen_model_template.py  addestra una RNN a partire dal file test.csv e salva modello e pesi in kdgi/build/py/models/test.txt e kdgi/build/py/models/test.hdf5
Il codice in kdgi/build/py/gen_dfa_template.py importa la RNN "test" e da essa estrae i dfa tramite algoritmi di AL.

BUG:

1) Per evitare di ricevere informazioni inconsistenti dagli oracoli sono state implementate delle funzioni per verificare se una membership query può essere ricavata dalle strutture dati dei learner.
Quella dell'AngluinDFA funziona correttamente, mentre quelle del TTTDFA in alcuni casi portano ad una eccezione.
Per questo motivo i learner mantengono memoria di tutte le membership query effettuate e dei relativi risultati, in modo che la stessa mem. query non venga posta 2 volte all'oracolo.
Per far verificare l'eccezione si possono seguire le istruzioni contenute nel file kdgi/build/py/bug.py

2) In alcuni casi i dfa estratti da TTT e Ob. Pack a partire da oracoli RNN non sono quelli canonici del linguaggio (deve esserci un errore nell'implementazione dei 2 algoritmi).
Questo non permette all'oracolo RNN di procedere oltre (per poter valutare l'ipotesi si basa sul presupposto che ogni stato sia discriminabile da tutti gli altri).
Sarebbe bene capire cosa causa questo bug il quale non si è verificato quando a fare da oracolo era un DFA, e risolverlo alla base ma la cosa richiede di studiare a fondo gli algoritmi e come sono stati implementati.
In alternativa si potrebbe calcolare il DFA canonico a partire dal DFA ipotesi se questo non dovesse già essere canonico usando ad esempio il table filling.