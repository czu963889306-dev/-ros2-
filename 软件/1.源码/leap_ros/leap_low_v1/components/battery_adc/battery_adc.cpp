#include "battery_adc.h"

#include <math.h>

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

static const char *TAG = "BATTERY_ADC";
static constexpr adc_atten_t kBatteryAdcAtten = ADC_ATTEN_DB_12;
static constexpr adc_bitwidth_t kBatteryAdcBitwidth = ADC_BITWIDTH_DEFAULT;
// Hardware divider: 8.4V battery reads 2.6857V at GPIO3, with 0.0574mA divider leak.
// The measured divider ratio is used directly; 1% resistor tolerance remains measurement tolerance.
static constexpr float kDividerRatio = 8.4f / 2.6857f;
static constexpr float kBatteryEmptyVoltage = 6.0f;
static constexpr float kBatteryFullVoltage = 8.4f;
static constexpr int kSampleCount = 16;

BatteryAdc::BatteryAdc(adc_unit_t unit, adc_channel_t channel)
    : unit_(unit),
      channel_(channel),
      unit_handle_(nullptr),
      cali_handle_(nullptr),
      initialized_(false),
      calibrated_(false) {}

esp_err_t BatteryAdc::Init() {
  if (initialized_) {
    return ESP_OK;
  }

  adc_oneshot_unit_init_cfg_t unit_config = {};
  unit_config.unit_id = unit_;
  esp_err_t err = adc_oneshot_new_unit(&unit_config, &unit_handle_);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "failed to create ADC unit: %s", esp_err_to_name(err));
    return err;
  }

  adc_oneshot_chan_cfg_t channel_config = {};
  channel_config.atten = kBatteryAdcAtten;
  channel_config.bitwidth = kBatteryAdcBitwidth;
  err = adc_oneshot_config_channel(
      unit_handle_, channel_, &channel_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "failed to configure ADC channel: %s", esp_err_to_name(err));
    return err;
  }

  adc_cali_curve_fitting_config_t cali_config = {};
  cali_config.unit_id = unit_;
  cali_config.chan = channel_;
  cali_config.atten = kBatteryAdcAtten;
  cali_config.bitwidth = kBatteryAdcBitwidth;
  err = adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle_);
  calibrated_ = (err == ESP_OK);
  if (!calibrated_) {
    ESP_LOGW(TAG, "ADC calibration unavailable: %s", esp_err_to_name(err));
  }

  initialized_ = true;
  ESP_LOGI(TAG, "battery ADC initialized on unit %d channel %d, ratio %.4f",
           static_cast<int>(unit_), static_cast<int>(channel_), kDividerRatio);
  return ESP_OK;
}

BatteryAdcReading BatteryAdc::Read() {
  BatteryAdcReading reading = {};
  if (!initialized_) {
    return reading;
  }

  int raw_sum = 0;
  int mv_sum = 0;
  int valid_samples = 0;
  for (int i = 0; i < kSampleCount; ++i) {
    int raw = 0;
    esp_err_t err = adc_oneshot_read(
        unit_handle_, channel_, &raw);
    if (err != ESP_OK) {
      continue;
    }

    int mv = 0;
    if (calibrated_) {
      err = adc_cali_raw_to_voltage(cali_handle_, raw, &mv);
      if (err != ESP_OK) {
        continue;
      }
    } else {
      mv = static_cast<int>(lroundf((static_cast<float>(raw) / 4095.0f) * 3300.0f));
    }

    raw_sum += raw;
    mv_sum += mv;
    ++valid_samples;
  }

  if (valid_samples == 0) {
    return reading;
  }

  reading.raw = raw_sum / valid_samples;
  reading.adc_voltage_v = static_cast<float>(mv_sum) / static_cast<float>(valid_samples) / 1000.0f;
  reading.battery_voltage_v = reading.adc_voltage_v * kDividerRatio;
  reading.percentage = static_cast<uint8_t>(lroundf(EstimatePercentage(reading.battery_voltage_v)));
  reading.valid = true;
  return reading;
}

float BatteryAdc::EstimatePercentage(float battery_voltage_v) const {
  const float pct =
      ((battery_voltage_v - kBatteryEmptyVoltage) /
       (kBatteryFullVoltage - kBatteryEmptyVoltage)) *
      100.0f;
  if (pct < 0.0f) {
    return 0.0f;
  }
  if (pct > 100.0f) {
    return 100.0f;
  }
  return pct;
}
