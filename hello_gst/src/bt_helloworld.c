#include <gst/gst.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
	GstElement *pipeline;
	GstBus *bus;
	GstMessage *msg;

	/* Initialize gstreamer*/
	gst_init(&argc, &argv);

	/* build the pipeline */
	pipeline = gst_parse_launch("playbin uri=file:///home/tingqian/Videos/Tao-SAN-2012-05.mp4", NULL);

	/* Start playing */
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	sleep(2);

	GST_DEBUG_BIN_TO_DOT_FILE(pipeline, GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

	/* wait until error or EOS */
	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
									GST_MESSAGE_ERROR | GST_MESSAGE_EOS);


	/* Free resources */
	if(msg != NULL)
		gst_message_unref(msg);

	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
}
