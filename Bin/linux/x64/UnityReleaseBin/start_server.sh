#! /bin/sh

###############################################
#CONFIG
CUR_USER=`whoami`
CUR_PATH=`pwd`
PROF_FILE="server.prof"
PROF_FLAG=3

do_start_server()
{
	if [ $# -lt 1 ] || [ $# -gt 3 ]
	then
		echo -e "do_start_server FAILED! BAD ARGUMENT!"
		exit 0
	fi
	check_server_status $1
	if [ $? == 0 ]
	then 
		echo -e "server:$1 \t\t\t\033[0;31;1mAlready Running!\033[0m"
		return 0
	fi

	cd $CUR_PATH/$1
	`export CPUPROFILE=$PROF_FILE`
	if [ $PROF_FLAG -eq 1 ]
	then
		nohup env LD_PRELOAD=libprofiler.so CPUPROFILE=$1.prof ./$1 $2 $3 > /dev/null 2>&1 &
	elif [ $PROF_FLAG -eq 2 ]
	then
		valgrind --tool=memcheck --leak-check=full --show-reachable=yes --leak-resolution=med -v --log-file=$1.memck ./$1 $2 $3 > /dev/null 2>&1 &
	else 
		nohup ./$1 $2 $3 > /dev/null 2>&1 &
	fi
	
	sleep 2
	check_server_status $1
	if [ $? == 0 ]
	then 
		echo -e "start server:$1 \t\t\t\033[0;32;1mDone\033[0m"	
	else
		echo -e "start server:$1 \t\t\t\033[0;31;1mFailed\033[0m"
	fi
}

check_server_status()
{
	if ! [ $# -eq 1 ]
	then
		echo -e "check_server_status FAILED! BAD ARGUMENT!"
		exit 0
	fi
	PID=`pgrep -u $CUR_USER $1`
	#echo -e "check_server_status: user($CUR_USER) pid($PID) server($1)"
	if ! [ -z $PID ]
	then
		return 0
	else
		return 1
	fi
}

echo -e "\033[0;31;1m START SERVER \033[0m"

ulimit -c unlimited
ulimit -n 20480

for i in "843Server" "DataServer 1" "LoginServer 1" "AccountServer" "HallServer 1" "MasterServer" "RoomServer 1" "WorldServer 1"
do
	cd $CUR_PATH
	if [ -d `echo $i | awk '{print $1}'` ]
	then
		do_start_server $i
	fi
done

