#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sensors.h>

#include "temp.h"

static FILE *f = NULL;
static char sensors = 0;

static const sensors_chip_name *chip_name = NULL;
static const sensors_feature *feature;

char *cpu_feature_name = "temp1";
char *sys_feature_name = "temp2";

temperature_t t_type = CELCIUS;

static int cpu_feature = 0;
static int sys_feature = 0;

void temp_deinit() {
	if (f != NULL) {
		fclose(f);
	}

	if (sensors) {
		sensors_cleanup();
	}
}

void temp_init(const char *filename) {
	atexit(temp_deinit);

	f = fopen(filename, "r");
	if (f == NULL) {
		fprintf(stderr, "could not open configfile %s: %s\n", filename,
				strerror(errno));
		exit(1);
	}

	if (sensors_init(f)) {
		fprintf(stderr, "could not initialize sensors\n");
		exit(1);
	}

	sensors = 1;
}

void temp_getusage(unsigned int *cpu_temp, unsigned int *sys_temp) {
	double cpu = 0, sys = 0;
	const sensors_chip_name *name;
	int chip_nr = 0;
	const char *chipnm = "coretemp-isa-0000";

	while ((name = sensors_get_detected_chips (NULL, &chip_nr)))
	{
		static char buf[200];
		if (sensors_snprintf_chip_name (buf, 200, name) < 0)
		    printf ("could not print chip name\n");

		if (strncmp (buf, chipnm, strlen (chipnm)) == 0)
		{
		    int i = 0;
			struct sensors_subfeature *sf;
			double val;

		    while ( (feature = sensors_get_features (name, &i)) )
		    {
		        if (feature->type == SENSORS_FEATURE_TEMP)
			    {
			        printf ("got feature type temperature\n");
					char *label;

					if (! (label = sensors_get_label (name, feature)))
					{
					    fprintf (stderr, "ERROR: Can't get label of feature %s!\n",
						         feature->name);
					}
					else
					{
					    fprintf (stderr, "label of feature %s: %s\n", feature->name, label);
					}

					sf = sensors_get_subfeature (name, feature, SENSORS_SUBFEATURE_TEMP_INPUT);
					if (sf)
					{
					    sensors_get_value (name, sf->number, &val);

					    if ((strncmp (label, "Core 0", strlen ("Core 0"))) == 0)
						{
					        printf ("Core 0: %+6.1fC\n", val);
							cpu_feature = feature->number;
							printf ("feature number: %d\n", cpu_feature);
							cpu = val;
							chip_name = name;
						}
					    else if ((strncmp (label, "Core 1", strlen ("Core 1"))) == 0)
						{
					        printf ("Core 1: %+6.1fC\n", val);
							sys_feature = feature->number;
							printf ("feature number: %d\n", sys_feature);
							sys = val;
							chip_name = name;
						}
					}
			    }
			    else
			    {
			        printf ("ignoring feature type\n");
			    }
		    }
		}
	}

	if (t_type == FAHRENHEIT) {
		cpu = TO_FAHRENHEIT(cpu);
		sys = TO_FAHRENHEIT(sys);
	} else if (t_type == KELVIN) {
		cpu = TO_KELVIN(cpu);
		sys = TO_KELVIN(sys);
	}
	
	*cpu_temp = (unsigned int)(cpu);
	*sys_temp = (unsigned int)(sys);
}
