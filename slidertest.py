import streamlit as st
import pandas as pd
from datetime import datetime
import matplotlib.pyplot as plt
import time

st.title('Omron PID log viewer')

# Read the data from the 'test.dat' file
df = pd.read_csv('test.dat', sep='\t')

# Convert start and last dates to Unix timestamps
start_date = datetime.strptime(df.iloc[0, 0], '%m-%d %H:%M:%S')
last_date = datetime.strptime(df.iloc[-1, 0], '%m-%d %H:%M:%S')
start_timestamp = int(start_date.replace(year=2023).timestamp()) - 9 * 60 * 60 #Adjustment between JST and UnixTime
last_timestamp = int(last_date.replace(year=2023).timestamp()) - 9 * 60 * 60 #Adjustment between JST and UnixTime

# Create a slider for date and time range selection
selected_timestamp_range = st.slider(
    'Select unix time range to display',
    start_timestamp,
    last_timestamp,
    (start_timestamp, last_timestamp),
    key='selected_datetime_slider'
)

# Unpack the selected range to get the start and end timestamps
selected_start_timestamp = start_timestamp
selected_end_timestamp = last_timestamp
selected_start_timestamp, selected_end_timestamp = selected_timestamp_range

# Convert the selected timestamps back to datetime objects
selected_start_datetime = datetime.fromtimestamp(selected_start_timestamp)
selected_end_datetime = datetime.fromtimestamp(selected_end_timestamp)

# Format the selected datetimes separately for date and time
selected_start_date_str = selected_start_datetime.strftime('%m/%d')
selected_start_time_str = ' ' + selected_start_datetime.strftime('%H:%M')
selected_end_date_str = selected_end_datetime.strftime('%m/%d')
selected_end_time_str = ' ' + selected_end_datetime.strftime('%H:%M')

# Create a table to display the selected date and time range
selected_range_table = pd.DataFrame(
    {'Selected Date': [selected_start_date_str + selected_start_time_str,
                       selected_end_date_str + selected_end_time_str]},
    index=['start', 'end']
)

st.header('Selection time range to display log data')
# Display the selected range table
st.table(selected_range_table)


# Convert the date column to datetime format
df['#Date'] = pd.to_datetime(df['#Date'], format='%m-%d %H:%M:%S')

# Filter the DataFrame based on the selected time range
filtered_df = df[
    (df['time_t'] >= selected_start_timestamp) &
    (df['time_t'] <= selected_end_timestamp)
]

# Get the last temperature value from the filtered DataFrame
if not filtered_df.empty:
    las_temperature = filtered_df['temperature'].iloc[-1]
else:
    las_temperature = None  # or assign a default value

# Display the current temperature
st.header(':blue[Latest Temperature:]' + str(las_temperature))

# Plot the graph
fig, ax1 = plt.subplots()

# Plot Temperature on the left axis
ax1.plot(filtered_df['#Date'], filtered_df['temperature'], color='green', label='Temperature')
ax1.set_ylabel('Temperature', color='green')
ax1.tick_params(axis='y', labelcolor='green')
ax1.grid(axis='y', lw=0.5)

# Create a twin axis for MV (Manipulated Variable)
ax2 = ax1.twinx()
ax2.plot(filtered_df['#Date'], filtered_df['MV'], color='red', label='MV')
ax2.set_ylabel('MV', color='red', rotation=270)
ax2.yaxis.set_label_coords(1.13, 0.5)
ax2.tick_params(axis='y', labelcolor='red')

# Plot SV (Setpoint Variable) on the left axis
ax1.plot(filtered_df['#Date'], filtered_df['SV'], color='blue', label='SV')

# Set the x-axis label
ax1.set_xlabel('Date')

# Add legend
lines, labels = ax1.get_legend_handles_labels()
lines2, labels2 = ax2.get_legend_handles_labels()
ax2.legend(lines + lines2, labels + labels2, loc='lower right')

# Rotate the x-axis tick labels for better readability
plt.xticks(rotation=90)

# Display the graph
st.pyplot(fig)

# Round the values in the filtered DataFrame to one decimal place
filtered_df = filtered_df.round(1)

# Sort the DataFrame by index in descending order
filtered_df = filtered_df.sort_index(ascending=False)

# Display the filtered DataFrame
st.header('Table')
st.dataframe(filtered_df)

