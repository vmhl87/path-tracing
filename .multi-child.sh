rm -f "$1.raw"

echo "thread $1 done in TIME" | \
	sed "s/TIME/$( \
		(time cat scene.conf | sed "s/report/#report/" | sed "s/iname/#iname/" | \
			sed "s/rname/#rname/" | sed "s/report/#report/" | \
			sed "s/camera/camera\n\trname $1.raw\n\treport 0/" | \
			./run &> /dev/null) 2>&1 | \
			head -n 2 | tail -n 1 | \
			awk -F 'real\t' '{print $2}' \
	)/"
