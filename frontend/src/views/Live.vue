<template>
    <v-container fluid class="bg-surface d-flex justify-center">
      <v-row class="mx-auto" style="max-width: 1200px;">
        <v-col cols="9">
          <figure class="highcharts-figure">
            <div id="temperatureChart"></div>
          </figure>
        </v-col>
        <v-col cols="3">
          <v-card class="pa-4">
            <v-card-text>
              <v-sheet class="pa-2 mb-2 text-center" color="blue-darken-4">
                <strong>Temperature</strong>
                <div class="text-h4">{{ temperature }}</div>
              </v-sheet>
              <v-sheet class="pa-2 mb-2 text-center" color="pink-darken-4">
                <strong>Heat Index</strong>
                <div class="text-h4">{{ heatIndex }}</div>
              </v-sheet>
              <v-sheet class="pa-2 mb-2 text-center" color="green-darken-4">
                <strong>Humidity</strong>
                <div class="text-h4">{{ humidity }}</div>
              </v-sheet>
              <v-sheet class="pa-2 mb-2 text-center" color="orange-darken-4">
                <strong>Pressure</strong>
                <div class="text-h4">{{ pressure }} hPa</div>
              </v-sheet>
              <v-sheet class="pa-2 text-center" color="brown-darken-4">
                <strong>Soil Moisture</strong>
                <div class="text-h4">{{ soilMoisture }}%</div>
              </v-sheet>
            </v-card-text>
          </v-card>
        </v-col>
      </v-row>
    </v-container>
  </template>
  
  <script setup>
  import { ref, onMounted, watch, computed } from "vue";
  import Highcharts from "highcharts";
  import more from "highcharts/highcharts-more";
  import Exporting from "highcharts/modules/exporting";
  import { useMqttStore } from "@/store/mqttStore";
  import { storeToRefs } from "pinia";
  Exporting(Highcharts);
  more(Highcharts);
  
  const Mqtt = useMqttStore();
  const { payload } = storeToRefs(Mqtt);
  const tempChart = ref(null);
 // const payload = ref({ temperature: 0, humidity: 0, heatindex: 0, pressure: 0, soilMoisture: 0, timestamp: Date.now(),});

  const CreateCharts = async () => {
  console.log('Creating charts...');
  tempHiChart.value = Highcharts.chart('container', {
    chart: { zoomType: 'x' },
    title: { text: 'Air Temperature and Heat Index Analysis', align: 'left' },
    yAxis: {
      title: { text: 'Air Temperature & Heat Index', style: { color: '#000000' } },
      labels: { format: '{value} °C' }
    },
    xAxis: {
      type: 'datetime',
      title: { text: 'Time', style: { color: '#000000' } },
    },
    tooltip: { shared: true },
    series: [
      {
        name: 'Temperature',
        type: 'spline',
        data: [],
        turboThreshold: 0,
        color: Highcharts.getOptions().colors[0]
      },
      {
        name: 'Heat Index',
        type: 'spline',
        data: [],
        turboThreshold: 0,
        color: Highcharts.getOptions().colors[1]
      }
    ]
  });
  console.log('Charts created');
};

  
  onMounted(() => {
    createCharts();
    Mqtt.connect();
    setTimeout(() => {
      Mqtt.subscribe("620162321");
      Mqtt.subscribe("620162321_sub","/elet2415");
    }, 3000);
  });
  
  watch(payload, (data) => {
  console.log('Received payload:', data);  // Add logging to check the structure

  if (data && data.temperature !== undefined && data.heatindex !== undefined) {
    if (points.value > 0) {
      points.value--;
    } else {
      shift.value = true;
    }

    tempHiChart.value.series[0].addPoint(
      { y: parseFloat(data.temperature.toFixed(2)), x: data.timestamp * 1000 },
      true,
      shift.value
    );

    tempHiChart.value.series[1].addPoint(
      { y: parseFloat(data.heatindex.toFixed(2)), x: data.timestamp * 1000 },
      true,
      shift.value
    );
  }
});


  
  const temperature = computed(() => {
  if (payload.value && payload.value.temperature !== undefined) {
    return `${payload.value.temperature.toFixed(2)} °C`;
  }
  return 'N/A'; // or any default value
});

const humidity = computed(() => {
  if (payload.value && payload.value.humidity !== undefined) {
    return `${payload.value.humidity.toFixed(2)} %`;
  }
  return 'N/A'; // or any default value
});

const heatindex = computed(() => {
  if (payload.value && payload.value.heatindex !== undefined) {
    return `${payload.value.heatindex.toFixed(2)} °C`;
  }
  return 'N/A'; // or any default value
});

  </script>
  
  <style scoped>
  .highcharts-figure {
    margin: 0 auto;
  }
  .v-container {
    background-color: #121212;
    color: #fff;
  }
  </style>
  
  
