<template>
      <v-container fluid class="bg-surface">
        <v-row class="mx-auto" style="max-width: 1200px; padding: 1rem;">
            <v-col cols="12">
                <v-card class="pa-4">
                    <v-card-title>Date Range Selection</v-card-title>
                    <v-card-text>
                        <v-row>
                            <v-col cols="12" md="5">
                                <v-text-field
                                    v-model="start"
                                    label="Start date"
                                    type="date"
                                    density="compact"
                                    variant="outlined"
                                    :rules="[v => !!v || 'Start date is required']"
                                    :max="end || maxDate"
                                ></v-text-field>
                            </v-col>
                            <v-col cols="12" md="5">
                                <v-text-field
                                    v-model="end"
                                    label="End date"
                                    type="date"
                                    density="compact"
                                    variant="outlined"
                                    :rules="[v => !!v || 'End date is required']"
                                    :min="start"
                                    :max="maxDate"
                                ></v-text-field>
                            </v-col>
                            <v-col cols="12" md="2" class="d-flex align-center">
                                <v-btn
                                    @click="updateAnalysis"
                                    color="primary"
                                    :loading="loading"
                                    :disabled="!start || !end"
                                >
                                    Analyze
                                </v-btn>
                            </v-col>
                        </v-row>
                    </v-card-text>
                </v-card>
            </v-col>
        </v-row>

        <!-- Analysis Section -->
        <v-row class="mx-auto" style="max-width: 1200px;">
            <v-col cols="12">
                <v-card class="pa-4">
                    <v-card-title>Environmental Analysis</v-card-title>
                    <v-card-text>
                        <v-row>
                            <!-- Weather -->
                            <v-col cols="12" md="6">
                                <v-card variant="outlined" class="pa-3">
                                    <v-card-title class="text-h6">Weather </v-card-title>
                                    <v-card-text>
                                        <div class="d-flex justify-space-between mb-2">
                                            <span>Comfort Level:</span>
                                            <span :class="comfortLevelClass">{{ comfortLevel }}</span>
                                        </div>
                                        <div class="d-flex justify-space-between mb-2">
                                            <span>Feels Like:</span>
                                            <span>{{ feelsLike }}</span>
                                        </div>
                                        <div class="d-flex justify-space-between">
                                            <span>Comfort Score:</span>
                                            <v-progress-circular
                                                :model-value="comfortScore"
                                                :color="comfortScoreColor"
                                                :size="30"
                                            >
                                                {{ comfortScore }}
                                            </v-progress-circular>
                                        </div>
                                    </v-card-text>
                                </v-card>
                            </v-col>
                            <!-- Environmental Alerts -->
                            <v-col cols="12" md="6">
                                <v-card variant="outlined" class="pa-3">
                                    <v-card-title class="text-h6">Environmental Alerts</v-card-title>
                                    <v-card-text>
                                        <div v-for="(alert, index) in alerts" :key="index" class="d-flex align-center mb-2">
                                            <v-icon :color="alert.color" class="mr-2">{{ alert.icon }}</v-icon>
                                            <span>{{ alert.message }}</span>
                                        </div>
                                    </v-card-text>
                                </v-card>
                            </v-col>
                            <!-- Trend Analysis -->
                            <v-col cols="12" md="6">
                                <v-card variant="outlined" class="pa-3">
                                    <v-card-title class="text-h6">Trend Analysis</v-card-title>
                                    <v-card-text>
                                        <div class="d-flex justify-space-between mb-2">
                                            <span>Temperature Trend:</span>
                                            <span :class="temperatureTrendClass">{{ temperatureTrend }}</span>
                                        </div>
                                        <div class="d-flex justify-space-between mb-2">
                                            <span>Humidity Trend:</span>
                                            <span :class="humidityTrendClass">{{ humidityTrend }}</span>
                                        </div>
                                        <div class="d-flex justify-space-between">
                                            <span>Pressure Trend:</span>
                                            <span :class="pressureTrendClass">{{ pressureTrend }}</span>
                                        </div>
                                    </v-card-text>
                                </v-card>
                            </v-col>
                            <!-- Data Insights -->
                            <v-col cols="12" md="6">
                                <v-card variant="outlined" class="pa-3">
                                    <v-card-title class="text-h6">Data Insights</v-card-title>
                                    <v-card-text>
                                        <div class="d-flex justify-space-between mb-2">
                                            <span>Temperature Stability:</span>
                                            <span>{{ temperatureStability }}</span>
                                        </div>
                                        <div class="d-flex justify-space-between mb-2">
                                            <span>Humidity Pattern:</span>
                                            <span>{{ humidityPattern }}</span>
                                        </div>
                                        <div class="d-flex justify-space-between">
                                            <span>Environmental State:</span>
                                            <span>{{ environmentalState }}</span>
                                        </div>
                                    </v-card-text>
                                </v-card>
                            </v-col>
                        </v-row>
                    </v-card-text>
                </v-card>
            </v-col>
        </v-row>
    </v-container>
</template>

<script setup>
import { ref, computed, onMounted } from "vue";
import { useMqttStore } from '@/store/mqttStore';
import { storeToRefs } from "pinia";
import { useAppStore } from "@/store/appStore";

// Store setup
const Mqtt = useMqttStore();
const { payload } = storeToRefs(Mqtt);
const AppStore = useAppStore();

// Date handling
const start = ref("");
const end = ref("");
const loading = ref(false);
const maxDate = computed(() => {
    const today = new Date();
    return today.toISOString().split('T')[0];
});

// Connect to MQTT on component mount
onMounted(() => {
    Mqtt.connect();
    setTimeout(() => {
        Mqtt.subscribe("620162321");
        Mqtt.subscribe("620162321_sub", "/elet2415");
    }, 3000);
});

const updateAnalysis = async () => {
    if (!start.value || !end.value) return;
    
    loading.value = true;
    try {
        const startDate = new Date(start.value).getTime() / 1000;
        const endDate = new Date(end.value).getTime() / 1000;
        
        // Fetch and process data from backend
        await AppStore.getAllInRange(startDate, endDate);
        
    } catch (error) {
        console.error('Error fetching analysis data:', error);
    } finally {
        loading.value = false;
    }
};

// Analysis computed properties
const comfortLevel = computed(() => {
    if (!payload.value) return 'N/A';
    const temp = parseFloat(payload.value.temperatureDHT);
    const hum = parseFloat(payload.value.humidity);
    const hi = parseFloat(payload.value.heatIndex);

    if (temp < 15) return 'Cold';
    if (temp > 30) return 'Hot';
    if (hum > 80) return 'Humid';
    if (hi > temp + 5) return 'Uncomfortable';
    return 'Comfortable';
});

const comfortLevelClass = computed(() => {
    switch (comfortLevel.value) {
        case 'Cold': return 'text-primary';
        case 'Hot': return 'text-error';
        case 'Humid': return 'text-warning';
        case 'Uncomfortable': return 'text-error';
        default: return 'text-success';
    }
});

const feelsLike = computed(() => {
    if (!payload.value) return 'N/A';
    const temp = parseFloat(payload.value.temperatureDHT);
    const hi = parseFloat(payload.value.heatIndex);
    return `${hi.toFixed(1)}°C (${temp.toFixed(1)}°C actual)`;
});

const comfortScore = computed(() => {
    if (!payload.value) return 0;
    const temp = parseFloat(payload.value.temperatureDHT);
    const hum = parseFloat(payload.value.humidity);
    const hi = parseFloat(payload.value.heatIndex);
    
    let score = 100;
    
    // Temperature criteria
    if (temp < 15) score -= 20;
    if (temp > 30) score -= 30;
    
    // Humidity criteria
    if (hum > 80) score -= 25;
    if (hum < 30) score -= 15;
    
    // Heat index criteria
    if (hi > temp + 5) score -= 20;
    
    return Math.max(0, Math.min(100, score));
});

const comfortScoreColor = computed(() => {
    if (comfortScore.value >= 80) return 'success';
    if (comfortScore.value >= 60) return 'warning';
    return 'error';
});

const alerts = computed(() => {
    if (!payload.value) return [];
    const alerts = [];
    const temp = parseFloat(payload.value.temperatureDHT);
    const hum = parseFloat(payload.value.humidity);
    const soil = parseFloat(payload.value.soilMoisture);
    
    if (temp > 30) {
        alerts.push({
            icon: 'mdi-thermometer-high',
            color: 'error',
            message: 'High temperature alert'
        });
    }
    
    if (hum > 80) {
        alerts.push({
            icon: 'mdi-water-percent',
            color: 'warning',
            message: 'High humidity alert'
        });
    }
    
    if (soil < 30) {
        alerts.push({
            icon: 'mdi-water',
            color: 'error',
            message: 'Low soil moisture alert'
        });
    }
    
    return alerts;
});

const temperatureTrend = computed(() => {
    if (!payload.value) return 'N/A';
    const temp = parseFloat(payload.value.temperatureDHT);
    const hi = parseFloat(payload.value.heatIndex);
    
    if (hi > temp + 5) return 'Rising';
    if (hi < temp - 2) return 'Falling';
    return 'Stable';
});

const temperatureTrendClass = computed(() => {
    switch (temperatureTrend.value) {
        case 'Rising': return 'text-error';
        case 'Falling': return 'text-primary';
        default: return 'text-success';
    }
});

const humidityTrend = computed(() => {
    if (!payload.value) return 'N/A';
    const hum = parseFloat(payload.value.humidity);
    
    if (hum > 80) return 'High';
    if (hum < 30) return 'Low';
    return 'Ideal';
});

const humidityTrendClass = computed(() => {
    switch (humidityTrend.value) {
        case 'High': return 'text-warning';
        case 'Low': return 'text-error';
        default: return 'text-success';
    }
});

const pressureTrend = computed(() => {
    if (!payload.value) return 'N/A';
    const press = parseFloat(payload.value.pressure);
    
    if (press > 1020) return 'High';
    if (press < 980) return 'Low';
    return 'Normal';
});

const pressureTrendClass = computed(() => {
    switch (pressureTrend.value) {
        case 'High': return 'text-primary';
        case 'Low': return 'text-warning';
        default: return 'text-success';
    }
});

const temperatureStability = computed(() => {
    if (!payload.value) return 'N/A';
    const temp = parseFloat(payload.value.temperatureDHT);
    const hi = parseFloat(payload.value.heatIndex);
    const diff = Math.abs(hi - temp);
    
    if (diff < 2) return 'Very Stable';
    if (diff < 5) return 'Stable';
    return 'Unstable';
});

const humidityPattern = computed(() => {
    if (!payload.value) return 'N/A';
    const hum = parseFloat(payload.value.humidity);
    
    if (hum > 80) return 'Very Humid';
    if (hum > 60) return 'Moderately Humid';
    if (hum < 30) return 'Very Dry';
    return 'Balanced';
});

const environmentalState = computed(() => {
    if (!payload.value) return 'N/A';
    const temp = parseFloat(payload.value.temperatureDHT);
    const hum = parseFloat(payload.value.humidity);
    const soil = parseFloat(payload.value.soilMoisture);
    
    if (temp > 30 && hum > 80) return 'Tropical';
    if (temp < 15 && hum < 30) return 'Dry';
    if (soil < 30) return 'Drought Conditions';
    return 'Normal';
});
</script>

<style scoped>
.text-success {
    color: #4CAF50 !important;
}
.text-error {
    color: #F44336 !important;
}
.text-warning {
    color: #FFC107 !important;
}
.text-primary {
    color: #2196F3 !important;
}
</style>