#/bin/bash

mkdir -p mnt
./tagfs $PWD/images $PWD/mnt
sleep 1

echo -e "cartoon\nhobbit\ngentil" > src_bilbo_$$
./printtags mnt/bilbo.jpeg > res_bilbo_$$
diff src_bilbo_$$ res_bilbo_$$ > diff_bilbo_$$
if test -s diff_bilbo_$$
then
    echo "Test bilbo ... Failed"
    cat diff_bilbo_$$
else
    echo "Test bilbo ... Passed"
fi
rm -f src_bilbo_$$ res_bilbo_$$ diff_bilbo_$$

echo -e "gentil" > src_gru_$$
./printtags mnt/gru.jpeg > res_gru_$$
diff src_gru_$$ res_gru_$$ > diff_gru_$$
if test -s diff_gru_$$
then
    echo "Test gru ... Failed"
    cat diff_gru_$$
else
    echo "Test gru ... Passed"
fi
rm -f src_gru_$$ res_gru_$$ diff_gru_$$

echo -e "animal" > src_marmotte_$$
./printtags mnt/marmotte.jpeg > res_marmotte_$$
diff src_marmotte_$$ res_marmotte_$$ > diff_marmotte_$$
if test -s diff_marmotte_$$
then
    echo "Test marmotte ... Failed"
    cat diff_marmotte_$$
else
    echo "Test marmotte ... Passed"
fi
rm -f src_marmotte_$$ res_marmotte_$$ diff_marmotte_$$

echo -e "animal\nmechant" > src_coyote_$$
./printtags mnt/coyote.jpeg > res_coyote_$$
diff src_coyote_$$ res_coyote_$$ > diff_coyote_$$
if test -s diff_coyote_$$
then
    echo "Test coyote ... Failed"
    cat diff_coyote_$$
else
    echo "Test coyote ... Passed"
fi
rm -f src_coyote_$$ res_coyote_$$ diff_coyote_$$

echo -ne "" > src_ipb_$$
./printtags mnt/ipb.jpeg > res_ipb_$$
diff src_ipb_$$ res_ipb_$$ > diff_ipb_$$
if test -s diff_ipb_$$
then
    echo "Test ipb ... Failed"
    cat diff_ipb_$$
else
    echo "Test ipb ... Passed"
fi
rm -f src_ipb_$$ res_ipb_$$ diff_ipb_$$

echo -e "monty\nanimal\nmechant" > src_rabbit_$$
./printtags mnt/rabbit.jpeg > res_rabbit_$$
diff src_rabbit_$$ res_rabbit_$$ > diff_rabbit_$$
if test -s diff_rabbit_$$
then
    echo "Test rabbit ... Failed"
    cat diff_rabbit_$$
else
    echo "Test rabbit ... Passed"
fi
rm -f src_rabbit_$$ res_rabbit_$$ diff_rabbit_$$

fusermount -u mnt