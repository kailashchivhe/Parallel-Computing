##dynamic loop test

TEST13=$(./dynamic_sched 1 0 10 10000 1 1 thread 100 2> /dev/null)
if ./approx $TEST13 50;
then
    echo oktest13
else
    echo notok "./dynamic_sched 1 0 10 10000 1 1 thread 100" should give roughly "50"
    exit 1
fi

TEST14=$(./dynamic_sched 1 0 10 10000 1 4 thread 100 2> /dev/null)
if ./approx $TEST14 50;
then
    echo oktest14
else
    echo notok "./dynamic_sched 1 0 10 10000 1 4 thread 100" should give roughly "50"
    exit 1
fi

TEST15=$(./dynamic_sched 1 0 10 10000 1 8 thread 100 2> /dev/null)
if ./approx $TEST15 50;
then
    echo oktest15
else
    echo notok "./dynamic_sched 1 0 10 10000 1 8 thread 100" should give roughly "50"
    exit 1
fi

TEST16=$(./dynamic_sched 1 0 10 10000 1 16 thread 100 2> /dev/null)
if ./approx $TEST16 50;
then
    echo oktest16
else
    echo notok "./dynamic_sched 1 0 10 10000 1 16 thread 100" should give roughly "50"
    exit 1
fi

TEST17=$(./dynamic_sched 1 0 10 10000 1 1 chunk 100 2> /dev/null)
if ./approx $TEST17 50;
then
    echo oktest17
else
    echo notok "./dynamic_sched 1 0 10 10000 1 1 chunk 100" should give roughly "50"
    exit 1
fi

TEST18=$(./dynamic_sched 1 0 10 10000 1 4 chunk 100 2> /dev/null)
if ./approx $TEST18 50;
then
    echo oktest18
else
    echo notok "./dynamic_sched 1 0 10 10000 1 4 chunk 100" should give roughly "50"
    exit 1
fi

TEST19=$(./dynamic_sched 1 0 10 10000 1 8 chunk 100 2> /dev/null)
if ./approx $TEST19 50;
then
    echo oktest19
else
    echo notok "./dynamic_sched 1 0 10 10000 1 8 chunk 100" should give roughly "50"
    exit 1
fi


TEST20=$(./dynamic_sched 1 0 10 10000 1 16 chunk 100 2> /dev/null)
if ./approx $TEST20 50;
then
    echo oktest20
else
    echo notok "./dynamic_sched 1 0 10 10000 1 16 chunk " should give roughly "50"
    exit 1
fi