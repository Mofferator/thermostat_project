from bokeh.plotting import figure, output_file, show

def temp_plot(self, temp_data):

	# create a data source for the plot
	source = ColumnDataSource(temp_data)

	# get a list of the channels in the data
	channels = list(temp_data.columns)[1:]

	# create a figure for the plot
	p = figure(x_axis_type='datetime', sizing_mode='stretch_both')

	# for each channel, plot a line
	for channel in channels:
		p.line(x='date', y=channel, source=source, name=channel)

	return p