d3.csv("https://spamatica.com/tempdata.csv", function(err, rows){

  function unpack(rows, key) {
  return rows.map(function(row) { return row[key]; });
}

var trace1 = {
  type: "scatter",
  mode: "lines",
  name: 'Temp Ute',
  x: unpack(rows, 'date'),
  y: unpack(rows, 'temp_ute'),
  line: {color: '#17BECF'}
}

var trace2 = {
  type: "scatter",
  mode: "lines",
  name: 'Temp Inne',
  x: unpack(rows, 'date'),
  y: unpack(rows, 'temp_inne'),
  line: {color: '#7F7F7F'}
}

var data = [trace1,trace2];

var layout = {
  title: 'Vallen väder',
  width: 1024,
  height: 512,
  xaxis: {
    autorange: true,
    range: ['2023-01-01', '2023-12-16'],
    rangeselector: {buttons: [
        {
          count: 1,
          label: '1d',
          step: 'day',
          stepmode: 'backward'
        },
        {
          count: 7,
          label: '1w',
          step: 'day',
          stepmode: 'backward'
        },
        {
          count: 1,
          label: '1m',
          step: 'month',
          stepmode: 'backward'
        },
        {step: 'all'}
      ]},
    rangeslider: {range: ['2023-01-01', '2023-12-16']},
    type: 'date'
  },
  yaxis: {
    autorange: true,
    range: [-30.0, +30.0],
    type: 'linear'
  }
};

Plotly.newPlot('weathergraph', data, layout);
})

