# -*- coding: utf-8 -*-
'''*******************************************************************************************************************
*																													 *
* COPYRIGHT RESERVED, Robert Bosch GmbH, 2019. All rights reserved.													 *
* The reproduction, distribution and utilization of this document as well as the communication of its contents to	 *
* others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.	 *
* All rights reserved in the event of the grant of a patent, utility model or design.								 *
*																													 *
*******************************************************************************************************************'''
# Python script to plot results
# Enter the result file name in command line
import plotly.graph_objects as go
import plotly.express as px
from operator import truediv
import json
import sys

chart1 = go.Figure()
chart2 = go.Figure()

com_data = {}
Payload = []
Latency_0 = []
Latency_1 = []
Latency_2 = []
Latency_3 = []

Throughput_0 = []
Throughput_1 = []
Throughput_2 = []
Throughput_3 = []

Latency_Intradomain = []
Latency_Interdomain = []
text_values = []

#name of the test result file from commnd line
json_file = sys.argv[1]
file_text = json_file.replace('.json','')

def ThroughputCalulater(latency, payload):
    Latency_temp = list(filter(lambda num: num != 0, latency))
    return [(x*1.0)/y for x, y in zip(payload, Latency_temp)]

with open(json_file) as fileptr:
  com_data.update(json.load(fileptr))
# iterate for intradomain results
# 0--> udp_intradomain
# 1--> udp_interdomain
# 2--> aracom_method_intradomain
# 3--> aracom_method_interdomain

[Payload.append(com_data['protocols'][0]['entries'][item]['payload']) for item in range(len(com_data['protocols'][0]['entries']))]
#[Payload.append('_'+ str((com_data['protocols'][0]['entries'][item]['payload']))+'_') for item in range(len(com_data['protocols'][0]['entries']))]
[Latency_0.append(com_data['protocols'][0]['entries'][item]['latency']) for item in range(len(com_data['protocols'][0]['entries']))]

chart1.add_trace(
    go.Scatter(
        mode='lines+markers',
        name=com_data['protocols'][0]['name'],
        text=Latency_0,
        textposition = "top center",
        x=Payload,
        y=Latency_0
        ))

Throughput_0 = ThroughputCalulater(Latency_0,Payload)
chart2.add_trace(
    go.Scatter(
        mode='lines+markers',
        name=com_data['protocols'][0]['name'],
        text=Throughput_0,
        textposition = "top center",
        x=Payload,
        y=Throughput_0
        ))

[Payload.append(com_data['protocols'][1]['entries'][item]['payload']) for item in range(len(com_data['protocols'][1]['entries']))]
[Latency_1.append(com_data['protocols'][1]['entries'][item]['latency']) for item in range(len(com_data['protocols'][1]['entries']))]

chart1.add_trace(
    go.Scatter(
        mode='lines+markers',
        name=com_data['protocols'][1]['name'],
        text=Latency_1,
        textposition = "top center",
        x=Payload,
        y=Latency_1
        ))

Throughput_1 = ThroughputCalulater(Latency_1,Payload)
chart2.add_trace(
    go.Scatter(
        mode='lines+markers',
        name=com_data['protocols'][1]['name'],
        text=Throughput_1,
        textposition = "top center",
        x=Payload,
        y=Throughput_1
        ))

[Payload.append(com_data['protocols'][2]['entries'][item]['payload']) for item in range(len(com_data['protocols'][2]['entries']))]
[Latency_2.append(com_data['protocols'][2]['entries'][item]['latency']) for item in range(len(com_data['protocols'][2]['entries']))]

chart1.add_trace(
    go.Scatter(
        mode='lines+markers',
        name=com_data['protocols'][2]['name'],
        text=Latency_2,
        textposition = "top center",
        x=Payload,
        y=Latency_2
        ))

Throughput_2 = ThroughputCalulater(Latency_2,Payload)
chart2.add_trace(
    go.Scatter(
        mode='lines+markers',
        name=com_data['protocols'][2]['name'],
        text=Throughput_2,
        textposition = "top center",
        x=Payload,
        y=Throughput_2
        ))
    
[Payload.append(com_data['protocols'][3]['entries'][item]['payload']) for item in range(len(com_data['protocols'][3]['entries']))]
[Latency_3.append(com_data['protocols'][3]['entries'][item]['latency']) for item in range(len(com_data['protocols'][3]['entries']))]

chart1.add_trace(
    go.Scatter(
        mode='lines+markers',
        name=com_data['protocols'][3]['name'],
        text=Latency_3,
        textposition = "top center",
        x=Payload,
        y=Latency_3
        ))

Throughput_3 = ThroughputCalulater(Latency_3,Payload)
chart2.add_trace(
    go.Scatter(
        mode='lines+markers',
        name=com_data['protocols'][3]['name'],
        text=Throughput_3,
        textposition = "top center",
        x=Payload,
        y=Throughput_3
        ))

zip_object = zip(Latency_2, Latency_0)
for Latency_2, Latency_0 in zip_object:
    Latency_Intradomain.append(Latency_2-Latency_0)

chart1.add_trace(
    go.Scatter(
        mode='lines+markers',
        name='aracom_stack_Intradomain',
        text=Latency_Intradomain,
        textposition = "top center",
        x=Payload,
        y=Latency_Intradomain
        ))

zip_object = zip(Latency_3, Latency_1)
for Latency_3, Latency_1 in zip_object:
    Latency_Interdomain.append(Latency_3-Latency_1)

chart1.add_trace(
    go.Scatter(
        mode='lines+markers',
        name='aracom_stack_Interadomain',
        text=Latency_Interdomain,
        textposition = "top center",
        x=Payload,
        y=Latency_Interdomain
        ))

# Edit the layout
chart1.update_layout(title="roundtrips : {0}, env : {1}".format(
    com_data['setup']['roundtrips'], com_data['setup']['env']),
    xaxis_title='payload(bytes)',
    yaxis_title='time(µs)')

# Edit the layout
chart2.update_layout(title="roundtrips : {0}, env : {1}".format(
    com_data['setup']['roundtrips'], com_data['setup']['env']),
    xaxis_title='payload(bytes)',
    yaxis_title='throughput(Bytes/µs)')

chart1.write_html(file_text+'_letency.html', auto_open=True)
chart2.write_html(file_text+'_throughput.html', auto_open=True)
