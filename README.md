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

oppure (per il testing):
`cmake -G "Unix Makefiles" -Dtest=on ..`
`make && make test`

Se si vuole vedere quale specifico test è riuscito/fallito occorre eseguire, all'interno della cartella build:
`./runUnitTests`

Inoltre, il porting della libreria `gi` su python utilizza il progetto pybind11, aggiunto mediante il comando:

`git submodule add https://github.com/pybind/pybind11.git`

nella directory `py`

Per informazioni circa l'utilizzo di `git merge` e `git rebase` per condividere codice tra branches, leggere:
HOWTO_branching.txt
