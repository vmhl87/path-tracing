if [[ $(ps aux | grep feh | wc -l) -eq 1 ]]; then
	feh image.bmp &
fi

while true; do
	echo "running..."
	./run < scene.conf &>/dev/null
	convert image.bmp image.png
	echo done

	inotifywait -e close_write scene.conf &>/dev/null
done
