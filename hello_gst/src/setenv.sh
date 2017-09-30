#!/bin/sh

# Make sure we are sourced rather than executing in subshell
# so export environment can work after we return
if [ "$0" != "bash" ]
then
echo argv[0] is "$0" rather than "bash"
echo "please source this script!"
return 1
fi


gst_dot_dir=GST_DEBUG_DUMP_DOT_DIR

echo "SHLVL=${SHLVL}; pwd=`pwd`"

echo
mkdir -p gstDebugDump
export $gst_dot_dir=./gstDebugDump/
echo "$gst_dot_dir=`printenv $gst_dot_dir`"
echo
echo "----Tips----"
echo "1.Remember call following func/micro in your C code"
echo '    GST_DEBUG_BIN_TO_DOT_FILE(pipeline, GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");'
echo "2.View the graph using:"
echo "     dot -Tpng xxx/dot > xxx.png && xviewer xxx.png"
echo
echo "         Enjoy"

return 0
