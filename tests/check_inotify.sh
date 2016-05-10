#bin/bash

mkdir -p mnt
./tagfs $PWD/images $PWD/mnt
sleep 1

touch images/test.jpeg
if [ -f "mnt/test.jpeg" ]
then 
  echo "Test newfile ... Passed"
else
  echo "Test newfile ... Failed"
fi

mv images/test.jpeg images/toto.jpeg
sleep 1
if [ -f "mnt/toto.jpeg" ] && [ ! -f "mnt/test.jpeg" ]
then 
  echo "Test move ... Passed"
else
  echo "Test move ... Failed"
fi

ln mnt/toto.jpeg mnt/test
(cat <<EOF
[rabbit.jpeg]
monty
animal
mechant

[bilbo.jpeg]
cartoon
hobbit
gentil

[coyote.jpeg]
animal
mechant

[gru.jpeg]
gentil

[marmotte.jpeg]
animal

[toto.jpeg]
test

EOF
) > check_toto_$$

diff $PWD/images/.tags.new check_toto_$$ > diff_toto_$$
if test -s diff_toto_$$
then
    echo "Test modifs .tags.new ... Failed"
    cat diff_toto_$$
else
    echo "Test modifs .tags.new ... Passed"
fi
rm -f check_toto_$$ diff_toto_$$

rm images/toto.jpeg
if [ ! -f "mnt/toto.jpeg" ]
then 
  echo "Test remove ... Passed"
else
  echo "Test remove ... Failed"
fi

fusermount -u mnt