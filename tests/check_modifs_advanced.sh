#/bin/bash

export LANG=C
export LC_ALL=C

mkdir -p mnt
./tagfs $PWD/images $PWD/mnt
sleep 1

# check the ln functionality
## creation of new tag
ln mnt/ipb.jpeg mnt/foo/ 
## linking multiple tags
ln mnt/animal/coyote.jpeg mnt/gentil/cartoon/
## check that paths exist
if [ -f "mnt/foo/ipb.jpeg" ] && [ -f "mnt/gentil/coyote.jpeg" ] && [ -f "mnt/cartoon/coyote.jpeg" ]
then 
  echo "Test ln ... Passed"
else
  echo "Test ln ... Failed"
fi

# check the unlink functionality
rm mnt/mechant/coyote.jpeg
rm mnt/mechant/rabbit.jpeg
mv mnt/foo/ipb.jpeg mnt/oof/
## check that the tags mechant and foo has been removed
if [ "$(ls -A mnt/mechant)" ] || [ "$(ls -A mnt/foo)" ] || [ ! -f "mnt/oof/ipb.jpeg" ]
then 
  echo "Test unlink ... Failed"
else
  echo "Test unlink ... Passed"
fi

# check that the modifications have been written in .tags.new 
(cat <<EOF
[rabbit.jpeg]
monty
animal

[bilbo.jpeg]
cartoon
hobbit
gentil

[coyote.jpeg]
cartoon
gentil
animal

[gru.jpeg]
gentil

[marmotte.jpeg]
animal

[ipb.jpeg]
oof

EOF
) > check_intermediate_$$

diff $PWD/images/.tags.new check_intermediate_$$ > diff_intermediate_$$
if test -s diff_intermediate_$$
then
    echo "Test intermediate .tags.new ... Failed"
    cat diff_intermediate_$$
else
    echo "Test intermediate .tags.new ... Passed"
fi
rm -f check_intermediate_$$ diff_intermediate_$$

# check that mkdir/rmdir still functions
mkdir mnt/bar
ln mnt/bilbo.jpeg mnt/bar/
mkdir mnt/baz
ln mnt/bilbo.jpeg mnt/baz/
ln mnt/gru.jpeg mnt/baz/
rmdir mnt/baz

fusermount -u mnt

# check that the final .tags.new file is correct
(cat <<EOF
[rabbit.jpeg]
animal
monty

[bilbo.jpeg]
gentil
hobbit
cartoon
bar

[coyote.jpeg]
animal
gentil
cartoon

[gru.jpeg]
gentil

[marmotte.jpeg]
animal

[ipb.jpeg]
oof

EOF
) > check_final_$$

diff $PWD/images/.tags.new check_final_$$ > diff_final_$$
if test -s diff_final_$$
then
    echo "Test mkdir/rmdir ... Failed"
    cat diff_final_$$
else
    echo "Test mkdir/rmdir ... Passed"
fi
rm -f check_final_$$ diff_final_$$
