#!/bin/sh

export sh_name=$0
export sh_args="$*"
export sh_arg1=$1
export sh_arg2=$2
export sh_arg3=$3
export sh_arg4=$4

export sh_dir=$(pwd)
export arg_null="null"
export arg_blank="　" 
export arg_help="help"
export arg_build="build"
export arg_clear="clear"
export arg_start="start"

export proj_name="cpphelper"
export proj_bin="$proj_name.exe"

func_usage()
{
    arg_line=$1
    shift;
    arg_msg="$*"
    echo "input: $sh_name $sh_args"
    echo "line: $arg_line; message: $arg_msg"
    echo ""
}

func_clear()
{
    rm -fr $proj_bin
    echo ""
}

func_build()
{
    g++ -std=c++11 main.cpp  -L./ -ldl -o $proj_bin

    echo ""
}

func_start()
{
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./
	./$proj_bin
}

##############################脚本入口
case ${sh_arg1:=$arg_null} in
    $arg_null ) func_start; ;;
    $arg_help ) func_usage $LINENO "show usage and exit"; ;;
    $arg_build  ) shift; func_build $*; ;;
    $arg_clear  ) func_clear; ;;
    $arg_start  ) func_start; ;;
    *) func_usage $LINENO "unsupport action"; exit; ;;
esac;

exit;
