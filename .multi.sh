if [ -z "$1" ]; then
	echo "usage: .multi.sh <#threads>"
	exit 1
fi

rm -f .postprocess-cmd
touch .postprocess-cmd

spawn() {
	(bash .multi-child.sh $1 && echo read "$1.raw" >> .postprocess-cmd) &
}

for (( i=1; i<=$1; i++ )); do
	spawn $i
done

while true; do
	if [[ $(cat .postprocess-cmd | wc -l) -eq $1 ]]; then
		break
	fi

	sleep 1
done

cat scene.conf | rg exposure >> .postprocess-cmd
cat scene.conf | rg gamma >> .postprocess-cmd

echo write image.bmp >> .postprocess-cmd
echo done >> .postprocess-cmd

./postprocess < .postprocess-cmd

rm -f .postprocess-cmd

for (( i=1; i<=$1; i++ )); do
	rm "$i.raw"
done

convert image.bmp image.png
