#Questo file deve essere posizionato in una cartella con 2 sottocartelle: dot e pdf
#Il lancio del comando andrà a creare in pdf la stessa struttura delle sottocartelle di dot
#in cui i file .dot saranno sostituiti da .pdf

createpath (){
	pdfname="$1"
	path=$(echo "$pdfname" | cut -f 1 -d '/')  
	index=1
	while [ "$path" != "$pdfname" ]
	do
		if [ ! -d "$path" ]; then
			mkdir "$path"
		fi
		index=$((index + 1))
		path="$path/$(echo "$pdfname" | cut -f $index -d '/')"
	done
}

for dir in $(find dot -type d) ; do
	for dotfile in "$dir"/*.dot ; do
		#echo "$dotfile"
		pdffile=$(echo "$dotfile" | cut -f 1 -d '.'  | cut -f 2- -d '/')
		pdffile="pdf/$pdffile.pdf"
		if [ ! -f "$pdffile" ]; then
			echo "$pdffile"
			createpath $pdffile
    			dot -Tpdf $dotfile -o $pdffile
		fi
	done
done