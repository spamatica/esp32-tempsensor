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
  line: {color: '#17BECF', width: 3}
}

var trace2 = {
  type: "scatter",
  mode: "lines",
  name: 'Temp Inne',
  x: unpack(rows, 'date'),
  y: unpack(rows, 'temp_inne'),
  line: {color: '#7F7F7F', width: 3}
}

var data = [trace1,trace2];

const now = new Date(Date.now());
var numDays = 5;
const lastWeek = new Date(Date.now() - 1000 * 3600 * 24 * numDays);

console.log(now.toISOString());

console.log("WOWOWO");

var layout = {
  title: 'Vallen väder',
  width: 1300,
  height: 900,
  xaxis: {
    //autorange: true,
    range: [lastWeek, now], 
    rangeselector: {
      buttons: [
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
          stepmode: 'backward',
	  active: true
        },
        {
          count: 1,
          label: '1m',
          step: 'month',
          stepmode: 'backward'
        },
        {
		  step: 'all'
	}
      ]},
    // gives a slider
    //rangeslider: {range: ['2023-01-01', '2023-12-31']}, 
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

