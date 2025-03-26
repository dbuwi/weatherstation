<template>
    <v-container fluid class="bg-surface d-flex justify-center">
      <v-row class="mx-auto" style="max-width: 1400px;">
        <v-col cols="9">
          <v-row>
            <v-col cols="12">
              <figure class="highcharts-figure">
                <div id="temperatureChart"></div>
              </figure>
            </v-col>
            <v-col cols="12">
              <figure class="highcharts-figure">
                <div id="humidityChart"></div>
              </figure>
            </v-col>
            <v-col cols="12">
              <figure class="highcharts-figure">
                <div id="pressureAltitudeChart"></div>
              </figure>
            </v-col>
            <v-col cols="12">
              <figure class="highcharts-figure">
                <div id="soilMoistureChart"></div>
              </figure>
            </v-col>
          </v-row>
        </v-col>
        <v-col cols="3">
          <v-card class="pa-4">
            <v-card-text>
              <v-sheet class="pa-2 mb-2 text-center" color="blue-darken-4">
                <strong>Temperature (DHT)</strong>
                <div class="text-h4">{{ temperatureDHT }}</div>
              </v-sheet>
              <v-sheet class="pa-2 mb-2 text-center" color="blue-darken-2">
                <strong>Temperature (BMP)</strong>
                <div class="text-h4">{{ temperatureBMP }}</div>
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
              <v-sheet class="pa-2 mb-2 text-center" color="purple-darken-4">
                <strong>Altitude</strong>
                <div class="text-h4">{{ altitude }} m</div>
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
  const charts = ref({
    temperature: null,
    humidity: null,
    pressureAltitude: null,
    soilMoisture: null
  });
  const points = ref(50); 
  const shift = ref(false);

  const createCharts = async () => {
    console.log('Creating charts...');
    
    // Temperature Chart (including both sensors and heat index)
    charts.value.temperature = Highcharts.chart('temperatureChart', {
      chart: { zoomType: 'x' },
      title: { text: 'Temperature Measurements', align: 'left' },
      yAxis: {
        title: { text: 'Temperature (°C)', style: { color: '#000000' } },
        labels: { format: '{value} °C' }
      },
      xAxis: {
        type: 'datetime',
        title: { text: 'Time', style: { color: '#000000' } },
      },
      tooltip: { shared: true },
      series: [
        {
          name: 'DHT Temperature',
          type: 'spline',
          data: [],
          color: '#1976D2'
        },
        {
          name: 'BMP Temperature',
          type: 'spline',
          data: [],
          color: '#64B5F6'
        },
        {
          name: 'Heat Index',
          type: 'spline',
          data: [],
          color: '#E91E63'
        }
      ]
    });

    // Humidity Chart
    charts.value.humidity = Highcharts.chart('humidityChart', {
      chart: { zoomType: 'x' },
      title: { text: 'Humidity', align: 'left' },
      yAxis: {
        title: { text: 'Humidity (%)', style: { color: '#000000' } },
        labels: { format: '{value} %' }
      },
      xAxis: {
        type: 'datetime',
        title: { text: 'Time', style: { color: '#000000' } },
      },
      tooltip: { shared: true },
      series: [{
        name: 'Humidity',
        type: 'spline',
        data: [],
        color: '#2E7D32'
      }]
    });

    // Pressure and Altitude Chart
    charts.value.pressureAltitude = Highcharts.chart('pressureAltitudeChart', {
      chart: { zoomType: 'x' },
      title: { text: 'Pressure and Altitude', align: 'left' },
      yAxis: [{
        title: { text: 'Pressure (hPa)', style: { color: '#000000' } },
        labels: { format: '{value} hPa' }
      }, {
        title: { text: 'Altitude (m)', style: { color: '#000000' } },
        opposite: true
      }],
      xAxis: {
        type: 'datetime',
        title: { text: 'Time', style: { color: '#000000' } },
      },
      tooltip: { shared: true },
      series: [
        {
          name: 'Pressure',
          type: 'spline',
          data: [],
          color: '#F57C00'
        },
        {
          name: 'Altitude',
          type: 'spline',
          data: [],
          yAxis: 1,
          color: '#9C27B0'
        }
      ]
    });

    // Soil Moisture Chart
    charts.value.soilMoisture = Highcharts.chart('soilMoistureChart', {
      chart: { zoomType: 'x' },
      title: { text: 'Soil Moisture', align: 'left' },
      yAxis: {
        title: { text: 'Soil Moisture (%)', style: { color: '#000000' } },
        labels: { format: '{value} %' }
      },
      xAxis: {
        type: 'datetime',
        title: { text: 'Time', style: { color: '#000000' } },
      },
      tooltip: { shared: true },
      series: [{
        name: 'Soil Moisture',
        type: 'spline',
        data: [],
        color: '#5D4037'
      }]
    });
    
    console.log('Charts created');
  };

  onMounted(() => {
    createCharts();
    console.log('Connecting to MQTT...');
    Mqtt.connect();
    setTimeout(() => {
      console.log('Subscribing to MQTT topics...');
      Mqtt.subscribe("620162321");
    }, 3000);
  });
  
  watch(payload, (newData) => {
    console.log('Payload received:', newData);
    if (!newData || newData.Data !== "Sensor") return;

    try {
      const timestamp = Date.now();
      
      if (points.value > 0) {
        points.value--;
      } else {
        shift.value = true;
      }

      // Update Temperature Chart
      if (newData.temperatureDHT !== undefined) {
        const tempDHT = parseFloat(newData.temperatureDHT);
        console.log('Adding DHT temperature point:', tempDHT);
        charts.value.temperature.series[0].addPoint(
          { x: timestamp, y: tempDHT },
          false,
          shift.value
        );
      }
      if (newData.temperatureBMP !== undefined) {
        const tempBMP = parseFloat(newData.temperatureBMP);
        console.log('Adding BMP temperature point:', tempBMP);
        charts.value.temperature.series[1].addPoint(
          { x: timestamp, y: tempBMP },
          false,
          shift.value
        );
      }
      if (newData.heatIndex !== undefined) {
        const heatIdx = parseFloat(newData.heatIndex);
        console.log('Adding heat index point:', heatIdx);
        charts.value.temperature.series[2].addPoint(
          { x: timestamp, y: heatIdx },
          true,
          shift.value
        );
      }

      // Update Humidity Chart
      if (newData.humidity !== undefined) {
        const hum = parseFloat(newData.humidity);
        console.log('Adding humidity point:', hum);
        charts.value.humidity.series[0].addPoint(
          { x: timestamp, y: hum },
          true,
          shift.value
        );
      }

      // Update Pressure and Altitude Chart
      if (newData.pressure !== undefined) {
        const press = parseFloat(newData.pressure);
        console.log('Adding pressure point:', press);
        charts.value.pressureAltitude.series[0].addPoint(
          { x: timestamp, y: press },
          false,
          shift.value
        );
      }
      if (newData.altitude !== undefined) {
        const alt = parseFloat(newData.altitude);
        console.log('Adding altitude point:', alt);
        charts.value.pressureAltitude.series[1].addPoint(
          { x: timestamp, y: alt },
          true,
          shift.value
        );
      }

      // Update Soil Moisture Chart
      if (newData.soilMoisture !== undefined) {
        const soil = parseFloat(newData.soilMoisture);
        console.log('Adding soil moisture point:', soil);
        charts.value.soilMoisture.series[0].addPoint(
          { x: timestamp, y: soil },
          true,
          shift.value
        );
      }
    } catch (error) {
      console.error('Error updating charts:', error);
    }
  });

  // Computed properties for the dashboard
  const temperatureDHT = computed(() => {
    if (payload.value?.temperatureDHT !== undefined) {
      return `${payload.value.temperatureDHT.toFixed(2)} °C`;
    }
    return 'N/A';
  });

  const temperatureBMP = computed(() => {
    if (payload.value?.temperatureBMP !== undefined) {
      return `${payload.value.temperatureBMP.toFixed(2)} °C`;
    }
    return 'N/A';
  });

  const heatIndex = computed(() => {
    if (payload.value?.heatIndex !== undefined) {
      return `${payload.value.heatIndex.toFixed(2)} °C`;
    }
    return 'N/A';
  });

  const humidity = computed(() => {
    if (payload.value?.humidity !== undefined) {
      return `${payload.value.humidity.toFixed(2)} %`;
    }
    return 'N/A';
  });

  const pressure = computed(() => {
    if (payload.value?.pressure !== undefined) {
      return payload.value.pressure.toFixed(2);
    }
    return 'N/A';
  });

  const altitude = computed(() => {
    if (payload.value?.altitude !== undefined) {
      return payload.value.altitude.toFixed(2);
    }
    return 'N/A';
  });

  const soilMoisture = computed(() => {
    if (payload.value?.soilMoisture !== undefined) {
      return payload.value.soilMoisture.toFixed(2);
    }
    return 'N/A';
  });
  </script>
  
  <style scoped>
  .highcharts-figure {
    margin: 0 auto;
    margin-bottom: 1.5rem;
  }
  .v-container {
    background-color: #121212;
    color: #fff;
  }
  #temperatureChart,
  #humidityChart,
  #pressureAltitudeChart,
  #soilMoistureChart {
    height: 300px;
  }
  </style>
  
  
