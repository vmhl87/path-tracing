cat template.md | \

	sed "s/TIME/$( \
		(time ./run < scene.conf &> /dev/null) 2>&1 | \
		head -n 2 | tail -n 1 | \
		awk -F 'real\t' '{print $2}' \
	)/" | \

	sed "s/CPU/$( \
		neofetch --config none --stdout | \
		rg CPU | sed s/^CPU:\ // | \
		sed s/\ $// \
	)/" | \

	tee README.md | rg -v "^\s*$"
