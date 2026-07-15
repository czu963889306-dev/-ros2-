#include "system_globals.h"
#include "camera_i2c_client.h"
#include "wifi_app.h"
#include "msg/battery_msg.h"
#include "msg/gamepad_msg.h"
#include "msg/imu_msg.h"
#include "msg/lidar_msg.h"
#include "msg/motion_msg.h"
#include "msg/temperature_msg.h"
#include "msg/ultrasonic_msg.h"

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "esp_http_server.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_partition.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"

static const char *TAG = "WIFI_PROVISION";
static constexpr size_t kMaxScanResults = 20;
static constexpr size_t kDnsPacketMaxSize = 512;
static constexpr size_t kScanJsonSize = 4096;
static constexpr size_t kStatusJsonSize = 8192;

static SemaphoreHandle_t s_http_buffer_mutex = nullptr;
static wifi_ap_record_t s_scan_records[kMaxScanResults] = {};
static char s_scan_json[kScanJsonSize] = {};
static char s_status_json[kStatusJsonSize] = {};

static const char kProvisionHtmlTemplate[] = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>%s Wi-Fi 配网</title>
  <style>
    :root {
      --bg: linear-gradient(160deg, #e7f7f3 0%%, #f7faf8 40%%, #d8ecff 100%%);
      --panel: rgba(255, 255, 255, 0.92);
      --ink: #17313b;
      --subtle: #5b7280;
      --accent: #147d64;
      --accent-2: #0b5ed7;
      --line: rgba(23, 49, 59, 0.12);
    }
    * { box-sizing: border-box; }
    body {
      margin: 0;
      min-height: 100vh;
      font-family: "Segoe UI", "PingFang SC", sans-serif;
      background: var(--bg);
      color: var(--ink);
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 20px;
    }
    .shell {
      width: min(760px, 100%%);
      background: var(--panel);
      backdrop-filter: blur(14px);
      border: 1px solid rgba(255,255,255,0.65);
      border-radius: 24px;
      box-shadow: 0 24px 60px rgba(16, 39, 56, 0.16);
      overflow: hidden;
    }
    .hero {
      padding: 28px 28px 18px;
      background: linear-gradient(135deg, rgba(20,125,100,0.12), rgba(11,94,215,0.10));
      border-bottom: 1px solid var(--line);
    }
    .hero-bar {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 12px;
      flex-wrap: wrap;
    }
    .eyebrow {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      padding: 6px 12px;
      border-radius: 999px;
      background: rgba(20,125,100,0.12);
      color: var(--accent);
      font-size: 13px;
      font-weight: 700;
      letter-spacing: 0.04em;
      text-transform: uppercase;
    }
    .lang-switch {
      display: inline-flex;
      align-items: center;
      gap: 8px;
    }
    .lang-btn {
      appearance: none;
      border: 1px solid rgba(23,49,59,0.12);
      background: rgba(255,255,255,0.72);
      color: var(--ink);
      border-radius: 999px;
      padding: 8px 12px;
      font-size: 12px;
      font-weight: 700;
      cursor: pointer;
    }
    .lang-btn.active {
      background: var(--accent);
      border-color: var(--accent);
      color: white;
    }
    h1 {
      margin: 14px 0 8px;
      font-size: clamp(28px, 4vw, 40px);
      line-height: 1.05;
    }
    p {
      margin: 0;
      color: var(--subtle);
      line-height: 1.6;
    }
    .grid {
      display: grid;
      grid-template-columns: 1.05fr 0.95fr;
    }
    .pane {
      padding: 24px 28px 28px;
    }
    .pane + .pane {
      border-left: 1px solid var(--line);
    }
    .row {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 12px;
      margin-bottom: 12px;
    }
    .section-title {
      margin: 0;
      font-size: 16px;
      font-weight: 700;
    }
    .ghost {
      appearance: none;
      border: 1px solid rgba(11, 94, 215, 0.16);
      background: rgba(11, 94, 215, 0.07);
      color: var(--accent-2);
      border-radius: 999px;
      padding: 10px 14px;
      font-weight: 700;
      cursor: pointer;
    }
    .hint {
      margin-bottom: 14px;
      font-size: 13px;
      color: var(--subtle);
    }
    .list {
      display: grid;
      gap: 10px;
      max-height: 320px;
      overflow: auto;
      padding-right: 2px;
    }
    .network {
      appearance: none;
      width: 100%%;
      border: 1px solid var(--line);
      border-radius: 16px;
      background: white;
      padding: 14px 16px;
      text-align: left;
      cursor: pointer;
      transition: transform 120ms ease, box-shadow 120ms ease, border-color 120ms ease;
    }
    .network:active {
      transform: scale(0.99);
    }
    .network:hover {
      border-color: rgba(20,125,100,0.34);
      box-shadow: 0 10px 24px rgba(20,125,100,0.10);
    }
    .network strong {
      display: block;
      font-size: 15px;
      color: var(--ink);
    }
    .network span {
      display: block;
      margin-top: 4px;
      font-size: 12px;
      color: var(--subtle);
    }
    .status {
      margin-bottom: 12px;
      font-size: 13px;
      color: var(--subtle);
    }
    .empty {
      padding: 18px;
      border: 1px dashed var(--line);
      border-radius: 16px;
      color: var(--subtle);
      background: rgba(255,255,255,0.55);
    }
    form {
      display: grid;
      gap: 14px;
    }
    label {
      display: block;
      font-size: 13px;
      font-weight: 700;
      margin-bottom: 6px;
    }
    input {
      width: 100%%;
      border: 1px solid var(--line);
      border-radius: 14px;
      padding: 13px 14px;
      font-size: 16px;
      background: rgba(255,255,255,0.95);
    }
    select {
      width: 100%%;
      border: 1px solid var(--line);
      border-radius: 14px;
      padding: 13px 14px;
      font-size: 16px;
      background: rgba(255,255,255,0.95);
    }
    .submit {
      appearance: none;
      border: 0;
      border-radius: 14px;
      padding: 14px 16px;
      background: linear-gradient(135deg, #147d64, #0b5ed7);
      color: white;
      font-size: 16px;
      font-weight: 800;
      letter-spacing: 0.01em;
      cursor: pointer;
      box-shadow: 0 14px 26px rgba(11,94,215,0.18);
    }
    .device {
      margin-top: 14px;
      padding: 12px 14px;
      border-radius: 14px;
      background: rgba(23,49,59,0.05);
      font-size: 13px;
      color: var(--subtle);
    }
    .device strong {
      color: var(--ink);
    }
    .link-value {
      color: var(--accent-2);
      text-decoration: none;
      font-weight: 700;
    }
    .link-value:hover {
      text-decoration: underline;
    }
    .config-panel {
      border-top: 1px solid var(--line);
      padding: 24px 28px 28px;
      background: rgba(255,255,255,0.54);
    }
    .dashboard {
      border-top: 1px solid var(--line);
      padding: 24px 28px 30px;
      background: rgba(255,255,255,0.46);
    }
    .status-head {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 12px;
      margin-bottom: 14px;
      flex-wrap: wrap;
    }
    .updated {
      font-size: 12px;
      color: var(--subtle);
    }
    .metrics {
      display: grid;
      grid-template-columns: repeat(4, minmax(0, 1fr));
      gap: 10px;
      margin-bottom: 14px;
    }
    .metric {
      border: 1px solid var(--line);
      border-radius: 12px;
      padding: 12px;
      background: rgba(255,255,255,0.82);
      min-width: 0;
    }
    .metric span {
      display: block;
      color: var(--subtle);
      font-size: 12px;
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
    }
    .metric strong {
      display: block;
      margin-top: 5px;
      font-size: 19px;
      line-height: 1.1;
      overflow-wrap: anywhere;
    }
    .status-grid {
      display: grid;
      grid-template-columns: repeat(3, minmax(0, 1fr));
      gap: 10px;
    }
    .status-panel {
      border: 1px solid var(--line);
      border-radius: 12px;
      padding: 12px;
      background: rgba(255,255,255,0.82);
      min-width: 0;
    }
    .status-panel h3 {
      margin: 0 0 10px;
      font-size: 14px;
    }
    .status-panel form {
      margin-top: 10px;
      gap: 10px;
    }
    .kv {
      display: flex;
      justify-content: space-between;
      gap: 10px;
      padding: 5px 0;
      border-top: 1px solid rgba(23,49,59,0.07);
      font-size: 12px;
    }
    .kv:first-of-type { border-top: 0; }
    .kv span {
      color: var(--subtle);
      white-space: nowrap;
    }
    .kv strong {
      text-align: right;
      overflow-wrap: anywhere;
    }
    .bar {
      height: 8px;
      border-radius: 999px;
      background: rgba(23,49,59,0.10);
      overflow: hidden;
      margin-top: 8px;
    }
    .bar i {
      display: block;
      height: 100%%;
      width: 0%%;
      background: linear-gradient(90deg, #147d64, #0b5ed7);
      border-radius: inherit;
      transition: width 180ms ease;
    }
    @media (max-width: 720px) {
      .grid { grid-template-columns: 1fr; }
      .pane + .pane { border-left: 0; border-top: 1px solid var(--line); }
      .hero, .pane { padding-left: 20px; padding-right: 20px; }
      .config-panel { padding-left: 20px; padding-right: 20px; }
      .dashboard { padding-left: 20px; padding-right: 20px; }
      .metrics { grid-template-columns: repeat(2, minmax(0, 1fr)); }
      .status-grid { grid-template-columns: 1fr; }
    }
  </style>
</head>
<body>
  <main class="shell">
    <section class="hero">
      <div class="hero-bar">
        <div id="portal-badge" class="eyebrow">配网门户已启用</div>
        <div class="lang-switch">
          <button id="lang-zh" class="lang-btn active" type="button">中文</button>
          <button id="lang-en" class="lang-btn" type="button">EN</button>
        </div>
      </div>
      <h1 id="hero-title" data-device-name="%s">正在为设备连接 Wi-Fi</h1>
      <p id="hero-desc">请保持连接当前热点。你可以从扫描结果中选择 Wi-Fi，也可以手动输入，保存后设备会自动重启并连接目标网络。</p>
    </section>
    <section class="grid">
      <div class="pane">
        <div class="row">
          <h2 id="nearby-title" class="section-title">附近 Wi-Fi</h2>
          <button id="scan-btn" class="ghost" type="button">重新扫描</button>
        </div>
        <div id="scan-status" class="status">正在扫描附近 Wi-Fi...</div>
        <div id="network-list" class="list">
          <div id="network-empty" class="empty">扫描结果会显示在这里。</div>
        </div>
      </div>
      <div class="pane">
        <div id="hint-text" class="hint">如果没有自动弹出配网页，请在连接当前热点后手动打开 <strong>http://192.168.4.1</strong>。</div>
        <form method="post" action="/api/provision">
          <input id="lang-input" name="lang" type="hidden" value="zh">
          <div>
            <label id="ssid-label" for="ssid">Wi-Fi 名称</label>
            <input id="ssid" name="ssid" maxlength="32" autocomplete="off" required>
          </div>
          <div>
            <label id="password-label" for="password">密码</label>
            <input id="password" name="password" maxlength="64" type="password" placeholder="开放网络可留空">
          </div>
          <button id="submit-btn" class="submit" type="submit">保存并重启</button>
        </form>
        <div id="device-label" class="device" data-device-name="%s">热点名称：<strong>%s</strong></div>
      </div>
    </section>
    <section class="config-panel">
      <div class="row">
        <h2 id="runtime-title" class="section-title">通信设置</h2>
        <div id="runtime-status" class="updated">加载中...</div>
      </div>
      <form id="runtime-form">
        <div>
          <label id="comm-mode-label" for="comm-mode">默认通信协议</label>
          <select id="comm-mode" name="comm_mode">
            <option value="micro_ros">micro-ROS</option>
            <option value="mavlink_udp">MAVLink UDP</option>
          </select>
        </div>
        <div>
          <label id="agent-ip-label" for="agent-ip">micro-ROS Agent IP</label>
          <input id="agent-ip" name="microros_agent_ip" maxlength="15" inputmode="decimal" required>
        </div>
        <div>
          <label id="agent-port-label" for="agent-port">micro-ROS Agent Port</label>
          <input id="agent-port" name="microros_agent_port" type="number" min="1" max="65535" required>
        </div>
        <button id="runtime-submit-btn" class="submit" type="submit">保存通信设置并重启</button>
      </form>
    </section>
    <section class="dashboard">
      <div class="status-head">
        <h2 id="status-title" class="section-title">设备状态</h2>
        <div id="status-updated" class="updated">等待状态数据...</div>
      </div>
      <div class="metrics">
        <div class="metric"><span id="metric-device-label">设备</span><strong id="metric-device">--</strong></div>
        <div class="metric"><span id="metric-wifi-label">Wi-Fi</span><strong id="metric-wifi">--</strong></div>
        <div class="metric"><span id="metric-temp-label">温度</span><strong id="metric-temp">--</strong></div>
        <div class="metric"><span id="metric-uptime-label">运行时间</span><strong id="metric-uptime">--</strong></div>
      </div>
      <div class="status-grid">
        <div class="status-panel">
          <h3 id="panel-system-title">系统</h3>
          <div class="kv"><span>Heap</span><strong id="stat-heap">--</strong></div>
          <div class="bar"><i id="bar-heap"></i></div>
          <div class="kv"><span>Flash</span><strong id="stat-flash">--</strong></div>
          <div class="bar"><i id="bar-flash"></i></div>
          <div class="kv"><span>Emergency</span><strong id="stat-emergency">--</strong></div>
          <div class="kv"><span>Motion Busy</span><strong id="stat-busy">--</strong></div>
        </div>
        <div class="status-panel">
          <h3 id="panel-imu-title">IMU</h3>
          <div class="kv"><span>Roll</span><strong id="stat-roll">--</strong></div>
          <div class="kv"><span>Pitch</span><strong id="stat-pitch">--</strong></div>
          <div class="kv"><span>Yaw</span><strong id="stat-yaw">--</strong></div>
          <div class="kv"><span>Accel</span><strong id="stat-accel">--</strong></div>
          <div class="kv"><span>Gyro</span><strong id="stat-gyro">--</strong></div>
        </div>
        <div class="status-panel">
          <h3 id="panel-motion-title">运动</h3>
          <div class="kv"><span>Velocity</span><strong id="stat-velocity">--</strong></div>
          <div class="kv"><span>Position</span><strong id="stat-position">--</strong></div>
          <div class="kv"><span>Wheel</span><strong id="stat-wheel">--</strong></div>
          <div class="kv"><span>Target</span><strong id="stat-target">--</strong></div>
          <div class="kv"><span>Mode</span><strong id="stat-motion-mode">--</strong></div>
        </div>
        <div class="status-panel">
          <h3 id="panel-sensor-title">传感器</h3>
          <div class="kv"><span>Ultrasonic</span><strong id="stat-ultrasonic">--</strong></div>
          <div class="kv"><span>Lidar</span><strong id="stat-lidar">--</strong></div>
          <div class="kv"><span>Gamepad</span><strong id="stat-gamepad">--</strong></div>
          <div class="kv"><span>Battery</span><strong id="stat-battery">--</strong></div>
        </div>
        <div class="status-panel">
          <h3 id="panel-wifi-title">网络</h3>
          <div class="kv"><span>Mode</span><strong id="stat-wifi-mode">--</strong></div>
          <div class="kv"><span>SSID</span><strong id="stat-wifi-ssid">--</strong></div>
          <div class="kv"><span>RSSI</span><strong id="stat-wifi-rssi">--</strong></div>
          <div class="kv"><span>AP IP</span><strong id="stat-ap-ip">--</strong></div>
        </div>
        <div class="status-panel">
          <h3 id="panel-camera-title">摄像头</h3>
          <div class="kv"><span>State</span><strong id="stat-camera-state">--</strong></div>
          <div class="kv"><span>Name</span><strong id="stat-camera-name">--</strong></div>
          <div class="kv"><span>IP</span><strong id="stat-camera-ip">--</strong></div>
          <div class="kv"><span>Config</span><strong id="stat-camera-config">--</strong></div>
          <div class="kv"><span>Stream</span><strong id="stat-camera-stream">--</strong></div>
          <form id="camera-form">
            <div>
              <label id="camera-ssid-label" for="camera-ssid">摄像头 Wi-Fi 名称</label>
              <input id="camera-ssid" name="ssid" maxlength="32" autocomplete="off" required>
            </div>
            <div>
              <label id="camera-password-label" for="camera-password">摄像头 Wi-Fi 密码</label>
              <input id="camera-password" name="password" maxlength="64" type="password" placeholder="开放网络可留空">
            </div>
            <div>
              <label id="camera-command-label" for="camera-command">配网命令格式</label>
              <select id="camera-command" name="format">
                <option value="wifi">WIFI:ssid,password</option>
                <option value="set_wifi">SET_WIFI:ssid,password</option>
                <option value="prov">PROV:ssid|password</option>
                <option value="json">{"ssid":"xxx","password":"yyy"}</option>
              </select>
            </div>
            <button id="camera-submit-btn" class="submit" type="submit">发送摄像头配网</button>
            <div id="camera-status" class="updated">等待摄像头状态...</div>
          </form>
        </div>
        <div class="status-panel">
          <h3 id="panel-pid-title">PID</h3>
          <div class="kv"><span>Speed</span><strong id="stat-pid-speed">--</strong></div>
          <div class="kv"><span>Position</span><strong id="stat-pid-position">--</strong></div>
        </div>
      </div>
    </section>
  </main>
  <script>
    const scanButton = document.getElementById('scan-btn');
    const langZhButton = document.getElementById('lang-zh');
    const langEnButton = document.getElementById('lang-en');
    const scanStatus = document.getElementById('scan-status');
    const networkList = document.getElementById('network-list');
    const langInput = document.getElementById('lang-input');
    const ssidInput = document.getElementById('ssid');
    const passwordInput = document.getElementById('password');
    const runtimeForm = document.getElementById('runtime-form');
    const commModeInput = document.getElementById('comm-mode');
    const agentIpInput = document.getElementById('agent-ip');
    const agentPortInput = document.getElementById('agent-port');
    const cameraForm = document.getElementById('camera-form');
    const cameraSsidInput = document.getElementById('camera-ssid');
    const cameraPasswordInput = document.getElementById('camera-password');
    const cameraCommandInput = document.getElementById('camera-command');
    const heroTitle = document.getElementById('hero-title');
    const deviceLabel = document.getElementById('device-label');
    const deviceName = heroTitle.dataset.deviceName || deviceLabel.dataset.deviceName || '';
    let currentLanguage = 'zh';
    let runtimeFormDirty = false;

    const translations = {
      zh: {
        documentTitle: `${deviceName} Wi-Fi 配网`,
        htmlLang: 'zh-CN',
        portalBadge: '配网门户已启用',
        heroTitle: `正在为 ${deviceName} 连接 Wi-Fi`,
        heroDesc: '请保持连接当前热点。你可以从扫描结果中选择 Wi-Fi，也可以手动输入，保存后设备会自动重启并连接目标网络。',
        nearbyTitle: '附近 Wi-Fi',
        scanAgain: '重新扫描',
        scanPending: '正在扫描附近 Wi-Fi...',
        emptyInitial: '扫描结果会显示在这里。',
        emptyNoNetworks: '没有扫描到可用 Wi-Fi。请让设备更靠近路由器后重试。',
        emptyScanFailed: 'Wi-Fi 扫描失败，请点击“重新扫描”再试一次。',
        hintHtml: '如果没有自动弹出配网页，请在连接当前热点后手动打开 <strong>http://192.168.4.1</strong>。',
        ssidLabel: 'Wi-Fi 名称',
        passwordLabel: '密码',
        passwordPlaceholder: '开放网络可留空',
        submit: '保存并重启',
        deviceHtml: `热点名称：<strong>${deviceName}</strong>`,
        foundStatus: (count) => `已发现 ${count} 个 Wi-Fi，点击任意一项可自动填入名称。`,
        scanFailedStatus: (message) => `扫描失败：${message}`,
        signalExcellent: '信号极强',
        signalStrong: '信号良好',
        signalFair: '信号一般',
        signalWeak: '信号较弱',
        authOpen: '开放网络',
        hiddenSsid: '(隐藏 SSID)',
        statusTitle: '设备状态',
        statusWaiting: '等待状态数据...',
        statusUpdated: (time) => `已更新 ${time}`,
        statusFailed: (message) => `状态读取失败：${message}`,
        runtimeTitle: '通信设置',
        runtimeLoading: '加载中...',
        runtimeSaved: '已保存，设备正在重启...',
        runtimeSaveFailed: (message) => `保存失败：${message}`,
        commModeLabel: '默认通信协议',
        agentIpLabel: 'micro-ROS Agent IP',
        agentPortLabel: 'micro-ROS Agent Port',
        runtimeSubmit: '保存通信设置并重启',
        cameraProvisionSaved: '摄像头配网命令已发送，等待摄像头重启或更新状态...',
        cameraProvisionFailed: (message) => `摄像头配网失败：${message}`,
        cameraSsidLabel: '摄像头 Wi-Fi 名称',
        cameraPasswordLabel: '摄像头 Wi-Fi 密码',
        cameraCommandLabel: '配网命令格式',
        cameraSubmit: '发送摄像头配网',
        cameraWaiting: '等待摄像头状态...',
        metricDevice: '设备',
        metricWifi: 'Wi-Fi',
        metricTemp: '温度',
        metricUptime: '运行时间',
        panelSystem: '系统',
        panelImu: 'IMU',
        panelMotion: '运动',
        panelSensor: '传感器',
        panelWifi: '网络',
        panelCamera: '摄像头',
        panelPid: 'PID',
        connected: '已连接',
        disconnected: '未连接',
        valid: '有效',
        invalid: '无数据',
        open: '打开',
        yes: '是',
        no: '否',
      },
      en: {
        documentTitle: `${deviceName} Wi-Fi Setup`,
        htmlLang: 'en',
        portalBadge: 'Captive Portal Active',
        heroTitle: `Connect ${deviceName} to Wi-Fi`,
        heroDesc: 'Stay connected to this hotspot. Choose a Wi-Fi network from the scan list or enter one manually, then save. The device will restart and join the selected network.',
        nearbyTitle: 'Nearby Wi-Fi',
        scanAgain: 'Scan Again',
        scanPending: 'Scanning nearby Wi-Fi...',
        emptyInitial: 'Scan results will appear here.',
        emptyNoNetworks: 'No Wi-Fi networks were found. Move the device closer to the router and scan again.',
        emptyScanFailed: 'Wi-Fi scan failed. Tap "Scan Again" to retry.',
        hintHtml: 'If the portal page does not open automatically, connect to this hotspot and manually open <strong>http://192.168.4.1</strong>.',
        ssidLabel: 'Wi-Fi Name',
        passwordLabel: 'Password',
        passwordPlaceholder: 'Leave empty for open networks',
        submit: 'Save and Restart',
        deviceHtml: `Hotspot name: <strong>${deviceName}</strong>`,
        foundStatus: (count) => `Found ${count} networks. Tap one to fill the SSID.`,
        scanFailedStatus: (message) => `Scan failed: ${message}`,
        signalExcellent: 'Excellent',
        signalStrong: 'Strong',
        signalFair: 'Fair',
        signalWeak: 'Weak',
        authOpen: 'Open',
        hiddenSsid: '(Hidden SSID)',
        statusTitle: 'Device Status',
        statusWaiting: 'Waiting for status data...',
        statusUpdated: (time) => `Updated ${time}`,
        statusFailed: (message) => `Status failed: ${message}`,
        runtimeTitle: 'Communication Settings',
        runtimeLoading: 'Loading...',
        runtimeSaved: 'Saved. Device is restarting...',
        runtimeSaveFailed: (message) => `Save failed: ${message}`,
        commModeLabel: 'Default Protocol',
        agentIpLabel: 'micro-ROS Agent IP',
        agentPortLabel: 'micro-ROS Agent Port',
        runtimeSubmit: 'Save Settings and Restart',
        cameraProvisionSaved: 'Camera provisioning command sent. Waiting for the camera to restart or update status...',
        cameraProvisionFailed: (message) => `Camera provisioning failed: ${message}`,
        cameraSsidLabel: 'Camera Wi-Fi Name',
        cameraPasswordLabel: 'Camera Wi-Fi Password',
        cameraCommandLabel: 'Provision Command Format',
        cameraSubmit: 'Send Camera Provisioning',
        cameraWaiting: 'Waiting for camera status...',
        metricDevice: 'Device',
        metricWifi: 'Wi-Fi',
        metricTemp: 'Temp',
        metricUptime: 'Uptime',
        panelSystem: 'System',
        panelImu: 'IMU',
        panelMotion: 'Motion',
        panelSensor: 'Sensors',
        panelWifi: 'Network',
        panelCamera: 'Camera',
        panelPid: 'PID',
        connected: 'Connected',
        disconnected: 'Disconnected',
        valid: 'Valid',
        invalid: 'No Data',
        open: 'Open',
        yes: 'Yes',
        no: 'No',
      }
    };

    function getI18n() {
      return translations[currentLanguage] || translations.zh;
    }

    function setProvisionLanguage(lang) {
      currentLanguage = translations[lang] ? lang : 'zh';
      const i18n = getI18n();

      document.documentElement.lang = i18n.htmlLang;
      document.title = i18n.documentTitle;
      document.getElementById('portal-badge').textContent = i18n.portalBadge;
      heroTitle.textContent = i18n.heroTitle;
      document.getElementById('hero-desc').textContent = i18n.heroDesc;
      document.getElementById('nearby-title').textContent = i18n.nearbyTitle;
      scanButton.textContent = i18n.scanAgain;
      document.getElementById('hint-text').innerHTML = i18n.hintHtml;
      document.getElementById('ssid-label').textContent = i18n.ssidLabel;
      document.getElementById('password-label').textContent = i18n.passwordLabel;
      passwordInput.placeholder = i18n.passwordPlaceholder;
      document.getElementById('submit-btn').textContent = i18n.submit;
      deviceLabel.innerHTML = i18n.deviceHtml;
      langInput.value = currentLanguage;
      document.getElementById('status-title').textContent = i18n.statusTitle;
      document.getElementById('runtime-title').textContent = i18n.runtimeTitle;
      document.getElementById('comm-mode-label').textContent = i18n.commModeLabel;
      document.getElementById('agent-ip-label').textContent = i18n.agentIpLabel;
      document.getElementById('agent-port-label').textContent = i18n.agentPortLabel;
      document.getElementById('runtime-submit-btn').textContent = i18n.runtimeSubmit;
      document.getElementById('camera-ssid-label').textContent = i18n.cameraSsidLabel;
      document.getElementById('camera-password-label').textContent = i18n.cameraPasswordLabel;
      document.getElementById('camera-command-label').textContent = i18n.cameraCommandLabel;
      document.getElementById('camera-submit-btn').textContent = i18n.cameraSubmit;
      cameraPasswordInput.placeholder = i18n.passwordPlaceholder;
      document.getElementById('metric-device-label').textContent = i18n.metricDevice;
      document.getElementById('metric-wifi-label').textContent = i18n.metricWifi;
      document.getElementById('metric-temp-label').textContent = i18n.metricTemp;
      document.getElementById('metric-uptime-label').textContent = i18n.metricUptime;
      document.getElementById('panel-system-title').textContent = i18n.panelSystem;
      document.getElementById('panel-imu-title').textContent = i18n.panelImu;
      document.getElementById('panel-motion-title').textContent = i18n.panelMotion;
      document.getElementById('panel-sensor-title').textContent = i18n.panelSensor;
      document.getElementById('panel-wifi-title').textContent = i18n.panelWifi;
      document.getElementById('panel-camera-title').textContent = i18n.panelCamera;
      document.getElementById('panel-pid-title').textContent = i18n.panelPid;

      langZhButton.classList.toggle('active', currentLanguage === 'zh');
      langEnButton.classList.toggle('active', currentLanguage === 'en');

      const url = new URL(window.location.href);
      url.searchParams.set('lang', currentLanguage);
      window.history.replaceState({}, '', url.toString());
    }

    function signalLabel(rssi) {
      const i18n = getI18n();
      if (rssi >= -55) return i18n.signalExcellent;
      if (rssi >= -67) return i18n.signalStrong;
      if (rssi >= -75) return i18n.signalFair;
      return i18n.signalWeak;
    }

    function pickNetwork(ssid) {
      ssidInput.value = ssid;
      cameraSsidInput.value = ssid;
      passwordInput.focus();
    }

    function renderNetworks(networks) {
      const i18n = getI18n();
      if (!Array.isArray(networks) || networks.length === 0) {
        networkList.innerHTML = `<div class="empty">${i18n.emptyNoNetworks}</div>`;
        return;
      }

      networkList.innerHTML = '';
      networks.forEach((network) => {
        const button = document.createElement('button');
        button.type = 'button';
        button.className = 'network';
        button.addEventListener('click', () => pickNetwork(network.ssid));

        const title = document.createElement('strong');
        title.textContent = network.ssid || i18n.hiddenSsid;
        button.appendChild(title);

        const meta = document.createElement('span');
        const auth = network.open ? i18n.authOpen : network.auth;
        meta.textContent = `${auth} | RSSI ${network.rssi} dBm | ${signalLabel(network.rssi)}`;
        button.appendChild(meta);

        networkList.appendChild(button);
      });
    }

    async function scanNetworks() {
      const i18n = getI18n();
      scanButton.disabled = true;
      scanStatus.textContent = i18n.scanPending;
      try {
        const response = await fetch('/api/scan', { cache: 'no-store' });
        if (!response.ok) {
          throw new Error(`HTTP ${response.status}`);
        }
        const payload = await response.json();
        renderNetworks(payload.networks || []);
        scanStatus.textContent = i18n.foundStatus(payload.networks ? payload.networks.length : 0);
      } catch (error) {
        networkList.innerHTML = `<div class="empty">${i18n.emptyScanFailed}</div>`;
        scanStatus.textContent = i18n.scanFailedStatus(error.message);
      } finally {
        scanButton.disabled = false;
      }
    }

    function text(id, value) {
      const node = document.getElementById(id);
      if (node) node.textContent = value;
    }

    function link(id, url) {
      const node = document.getElementById(id);
      if (!node) return;
      if (!url) {
        node.textContent = '--';
        return;
      }
      node.innerHTML = '';
      const anchor = document.createElement('a');
      anchor.className = 'link-value';
      anchor.href = url;
      anchor.target = '_blank';
      anchor.rel = 'noopener noreferrer';
      anchor.textContent = getI18n().open;
      node.appendChild(anchor);
    }

    function pct(value, total) {
      if (!total || total <= 0) return 0;
      return Math.max(0, Math.min(100, Math.round((value / total) * 100)));
    }

    function setBar(id, percent) {
      const node = document.getElementById(id);
      if (node) node.style.width = `${Math.max(0, Math.min(100, percent))}%%`;
    }

    function fixed(value, digits = 1) {
      return Number.isFinite(value) ? Number(value).toFixed(digits) : '--';
    }

    function formatBool(value) {
      const i18n = getI18n();
      return value ? i18n.yes : i18n.no;
    }

    function formatUptime(ms) {
      if (!Number.isFinite(ms)) return '--';
      const totalSeconds = Math.floor(ms / 1000);
      const hours = Math.floor(totalSeconds / 3600);
      const minutes = Math.floor((totalSeconds %% 3600) / 60);
      const seconds = totalSeconds %% 60;
      if (hours > 0) return `${hours}h ${minutes}m`;
      if (minutes > 0) return `${minutes}m ${seconds}s`;
      return `${seconds}s`;
    }

    function pidLine(pid) {
      if (!pid) return '--';
      return `${fixed(pid.kp, 2)} / ${fixed(pid.ki, 2)} / ${fixed(pid.kd, 2)}`;
    }

    function renderStatus(data) {
      const i18n = getI18n();
      const wifi = data.wifi || {};
      const memory = data.memory || {};
      const storage = data.storage || {};
      const temperature = data.temperature || {};
      const imu = data.imu || {};
      const motion = data.motion || {};
      const ultrasonic = data.ultrasonic || {};
      const lidar = data.lidar || {};
      const gamepad = data.gamepad || {};
      const battery = data.battery || {};
      const pid = data.pid || {};
      const runtime = data.runtime || {};
      const camera = data.camera || {};

      text('metric-device', data.device_name || '--');
      text('metric-wifi', wifi.sta_connected ? i18n.connected : `${wifi.mode || '--'}`);
      text('metric-temp', temperature.valid ? `${fixed(temperature.celsius, 1)} C` : '--');
      text('metric-uptime', formatUptime(data.uptime_ms));
      text('status-updated', i18n.statusUpdated(new Date().toLocaleTimeString()));

      const heapPercent = pct(memory.heap_used_kib, memory.heap_total_kib);
      text('stat-heap', `${memory.heap_used_kib || 0}/${memory.heap_total_kib || 0} KiB (${heapPercent}%%)`);
      setBar('bar-heap', heapPercent);
      const flashPercent = pct(storage.partition_used_kib, storage.flash_total_kib);
      text('stat-flash', `${storage.partition_used_kib || 0}/${storage.flash_total_kib || 0} KiB (${flashPercent}%%)`);
      setBar('bar-flash', flashPercent);
      text('stat-emergency', formatBool(data.emergency_stop));
      text('stat-busy', formatBool(data.motion_busy));

      text('stat-roll', imu.valid ? `${fixed(imu.roll, 1)} deg` : i18n.invalid);
      text('stat-pitch', imu.valid ? `${fixed(imu.pitch, 1)} deg` : i18n.invalid);
      text('stat-yaw', imu.valid ? `${fixed(imu.yaw, 1)} deg` : i18n.invalid);
      text('stat-accel', imu.valid ? `${fixed(imu.acc_x, 2)}, ${fixed(imu.acc_y, 2)}, ${fixed(imu.acc_z, 2)} g` : i18n.invalid);
      text('stat-gyro', imu.valid ? `${fixed(imu.gyro_x, 1)}, ${fixed(imu.gyro_y, 1)}, ${fixed(imu.gyro_z, 1)} dps` : i18n.invalid);

      text('stat-velocity', motion.valid ? `${fixed(motion.vx, 0)}, ${fixed(motion.vy, 0)} mm/s | ${fixed(motion.wz, 2)} rad/s` : i18n.invalid);
      text('stat-position', motion.valid ? `${fixed(motion.x, 0)}, ${fixed(motion.y, 0)} mm | ${fixed(motion.yaw, 1)} deg` : i18n.invalid);
      text('stat-wheel', motion.valid ? `${fixed(motion.vel_left, 0)} / ${fixed(motion.vel_right, 0)} mm/s` : i18n.invalid);
      text('stat-target', motion.valid ? `${fixed(motion.target_vx, 0)}, ${fixed(motion.target_vy, 0)} | ${fixed(motion.target_wz, 2)}` : i18n.invalid);
      text('stat-motion-mode', motion.valid ? `${motion.control_mode} / src ${motion.source}` : i18n.invalid);

      text('stat-ultrasonic', ultrasonic.valid ? `${fixed(ultrasonic.distance_cm, 1)} cm | ${ultrasonic.obstacle ? 'OBS' : 'clear'}` : i18n.invalid);
      text('stat-lidar', lidar.valid ? `${lidar.valid_points} pts | ${lidar.min_mm}-${lidar.max_mm} mm` : i18n.invalid);
      text('stat-gamepad', gamepad.valid ? `${gamepad.connected ? i18n.connected : i18n.disconnected} | ${gamepad.left_x},${gamepad.left_y} / ${gamepad.right_x},${gamepad.right_y}` : i18n.invalid);
      text('stat-battery', battery.valid ? `${fixed(battery.voltage_v, 2)} V | ${battery.percentage}%% | ADC ${fixed(battery.adc_voltage_v, 3)} V` : i18n.invalid);

      text('stat-wifi-mode', wifi.mode || '--');
      text('stat-wifi-ssid', wifi.ssid || '--');
      text('stat-wifi-rssi', Number.isFinite(wifi.rssi) ? `${wifi.rssi} dBm` : '--');
      text('stat-ap-ip', wifi.ap_ip || '--');

      text('stat-camera-state', camera.reachable ? (camera.valid ? i18n.connected : i18n.invalid) : i18n.disconnected);
      text('stat-camera-name', camera.name || '--');
      text('stat-camera-ip', camera.ip || '--');
      link('stat-camera-config', camera.config_url);
      link('stat-camera-stream', camera.stream_url);
      text('camera-status', camera.reachable ? `${camera.mode || '--'} | ${camera.ip || '--'}` : (camera.error || i18n.disconnected));

      text('stat-pid-speed', pidLine(pid.speed));
      text('stat-pid-position', pidLine(pid.position));

      if (!runtimeFormDirty) {
        if (runtime.comm_mode) commModeInput.value = runtime.comm_mode;
        if (runtime.microros_agent_ip) agentIpInput.value = runtime.microros_agent_ip;
        if (runtime.microros_agent_port) agentPortInput.value = runtime.microros_agent_port;
      }
      text('runtime-status', `${runtime.comm_mode || '--'} | ${runtime.microros_agent_ip || '--'}:${runtime.microros_agent_port || '--'}`);
    }

    async function refreshStatus() {
      try {
        const response = await fetch('/api/status', { cache: 'no-store' });
        if (!response.ok) {
          throw new Error(`HTTP ${response.status}`);
        }
        renderStatus(await response.json());
      } catch (error) {
        text('status-updated', getI18n().statusFailed(error.message));
      }
    }

    window.setProvisionLanguage = setProvisionLanguage;
    langZhButton.addEventListener('click', () => setProvisionLanguage('zh'));
    langEnButton.addEventListener('click', () => setProvisionLanguage('en'));
    scanButton.addEventListener('click', scanNetworks);
    runtimeForm.addEventListener('input', () => {
      runtimeFormDirty = true;
    });
    runtimeForm.addEventListener('submit', async (event) => {
      event.preventDefault();
      const i18n = getI18n();
      const body = new URLSearchParams();
      body.set('comm_mode', commModeInput.value);
      body.set('microros_agent_ip', agentIpInput.value.trim());
      body.set('microros_agent_port', agentPortInput.value.trim());
      try {
        const response = await fetch('/api/runtime-config', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body
        });
        if (!response.ok) {
          throw new Error(await response.text() || `HTTP ${response.status}`);
        }
        runtimeFormDirty = false;
        text('runtime-status', i18n.runtimeSaved);
      } catch (error) {
        text('runtime-status', i18n.runtimeSaveFailed(error.message));
      }
    });
    cameraForm.addEventListener('submit', async (event) => {
      event.preventDefault();
      const i18n = getI18n();
      const body = new URLSearchParams();
      body.set('ssid', cameraSsidInput.value.trim());
      body.set('password', cameraPasswordInput.value);
      body.set('format', cameraCommandInput.value);
      try {
        const response = await fetch('/api/camera/provision', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body
        });
        if (!response.ok) {
          throw new Error(await response.text() || `HTTP ${response.status}`);
        }
        text('camera-status', i18n.cameraProvisionSaved);
        refreshStatus();
      } catch (error) {
        text('camera-status', i18n.cameraProvisionFailed(error.message));
      }
    });
    window.addEventListener('load', () => {
      const lang = new URLSearchParams(window.location.search).get('lang');
      setProvisionLanguage(lang === 'en' ? 'en' : 'zh');
      document.getElementById('network-empty').textContent = getI18n().emptyInitial;
      document.getElementById('camera-status').textContent = getI18n().cameraWaiting;
      scanNetworks();
      refreshStatus();
      setInterval(refreshStatus, 1000);
    });
  </script>
</body>
</html>
)HTML";

static void restart_after_delay_task(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(1200));
    esp_restart();
}

static int hex_to_int(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    return -1;
}

static void url_decode(const char *src, char *dst, size_t dst_size) {
    if (dst_size == 0) {
        return;
    }

    size_t di = 0;
    for (size_t si = 0; src[si] != '\0' && di + 1 < dst_size; ++si) {
        if (src[si] == '+') {
            dst[di++] = ' ';
        } else if (src[si] == '%' && isxdigit(static_cast<unsigned char>(src[si + 1])) &&
                   isxdigit(static_cast<unsigned char>(src[si + 2]))) {
            const int hi = hex_to_int(src[si + 1]);
            const int lo = hex_to_int(src[si + 2]);
            if (hi >= 0 && lo >= 0) {
                dst[di++] = static_cast<char>((hi << 4) | lo);
                si += 2;
            }
        } else {
            dst[di++] = src[si];
        }
    }
    dst[di] = '\0';
}

static bool extract_form_field(const char *body, const char *key, char *out, size_t out_size) {
    const size_t key_len = strlen(key);
    const char *cursor = body;
    while (cursor != NULL && *cursor != '\0') {
        const bool key_matches = strncmp(cursor, key, key_len) == 0 && cursor[key_len] == '=';
        if (key_matches) {
            const char *value_start = cursor + key_len + 1;
            const char *value_end = strchr(value_start, '&');
            const size_t raw_len = (value_end == NULL) ? strlen(value_start)
                                                       : static_cast<size_t>(value_end - value_start);

            char raw_value[256] = {0};
            const size_t copy_len =
                (raw_len < sizeof(raw_value) - 1) ? raw_len : sizeof(raw_value) - 1;
            memcpy(raw_value, value_start, copy_len);
            raw_value[copy_len] = '\0';
            url_decode(raw_value, out, out_size);
            return true;
        }

        cursor = strchr(cursor, '&');
        if (cursor != NULL) {
            ++cursor;
        }
    }

    if (out_size > 0) {
        out[0] = '\0';
    }
    return false;
}

static bool is_lang_en(const char *lang) {
    return lang != NULL && (strcmp(lang, "en") == 0 || strcmp(lang, "en-US") == 0);
}

static bool parse_comm_mode(const char *value, WifiCommMode *mode) {
    if (value == NULL || mode == NULL) {
        return false;
    }
    if (strcmp(value, "micro_ros") == 0 || strcmp(value, "microros") == 0) {
        *mode = WifiCommMode::kMicroRos;
        return true;
    }
    if (strcmp(value, "mavlink_udp") == 0 || strcmp(value, "mavlink") == 0) {
        *mode = WifiCommMode::kMavlinkUdp;
        return true;
    }
    return false;
}

static bool parse_camera_provision_format(const char *value, camera_i2c_provision_format_t *format) {
    if (value == NULL || format == NULL) {
        return false;
    }
    if (strcmp(value, "wifi") == 0 || strcmp(value, "WIFI") == 0) {
        *format = CAMERA_I2C_PROVISION_WIFI;
        return true;
    }
    if (strcmp(value, "set_wifi") == 0 || strcmp(value, "SET_WIFI") == 0) {
        *format = CAMERA_I2C_PROVISION_SET_WIFI;
        return true;
    }
    if (strcmp(value, "prov") == 0 || strcmp(value, "PROV") == 0) {
        *format = CAMERA_I2C_PROVISION_PROV;
        return true;
    }
    if (strcmp(value, "json") == 0 || strcmp(value, "JSON") == 0) {
        *format = CAMERA_I2C_PROVISION_JSON;
        return true;
    }
    return false;
}

static bool is_valid_ipv4_address(const char *ip) {
    if (ip == NULL || ip[0] == '\0') {
        return false;
    }

    in_addr addr = {};
    return inet_pton(AF_INET, ip, &addr) == 1;
}

static bool append_text(char *buffer, size_t buffer_size, int *offset, const char *text) {
    if (buffer == NULL || offset == NULL || text == NULL || *offset < 0) {
        return false;
    }

    const int written = snprintf(buffer + *offset, buffer_size - static_cast<size_t>(*offset), "%s", text);
    if (written < 0 || static_cast<size_t>(*offset + written) >= buffer_size) {
        return false;
    }

    *offset += written;
    return true;
}

static bool append_format(char *buffer, size_t buffer_size, int *offset, const char *format, ...) {
    if (buffer == NULL || offset == NULL || format == NULL || *offset < 0) {
        return false;
    }

    va_list args;
    va_start(args, format);
    const int written =
        vsnprintf(buffer + *offset, buffer_size - static_cast<size_t>(*offset), format, args);
    va_end(args);

    if (written < 0 || static_cast<size_t>(*offset + written) >= buffer_size) {
        return false;
    }

    *offset += written;
    return true;
}

static bool append_json_string(char *buffer, size_t buffer_size, int *offset, const char *text) {
    if (!append_text(buffer, buffer_size, offset, "\"")) {
        return false;
    }

    for (const unsigned char *cursor = reinterpret_cast<const unsigned char *>(text);
         cursor != NULL && *cursor != '\0'; ++cursor) {
        if (*cursor == '\\' || *cursor == '"') {
            if (!append_format(buffer, buffer_size, offset, "\\%c", *cursor)) {
                return false;
            }
        } else if (*cursor < 0x20) {
            if (!append_format(buffer, buffer_size, offset, "\\u%04x", *cursor)) {
                return false;
            }
        } else {
            const char ch[2] = {static_cast<char>(*cursor), '\0'};
            if (!append_text(buffer, buffer_size, offset, ch)) {
                return false;
            }
        }
    }

    return append_text(buffer, buffer_size, offset, "\"");
}

static const char *auth_mode_to_string(wifi_auth_mode_t authmode) {
    switch (authmode) {
        case WIFI_AUTH_OPEN:
            return "OPEN";
        case WIFI_AUTH_WEP:
            return "WEP";
        case WIFI_AUTH_WPA_PSK:
            return "WPA";
        case WIFI_AUTH_WPA2_PSK:
            return "WPA2";
        case WIFI_AUTH_WPA_WPA2_PSK:
            return "WPA/WPA2";
#ifdef WIFI_AUTH_WPA2_ENTERPRISE
        case WIFI_AUTH_WPA2_ENTERPRISE:
            return "WPA2-ENT";
#endif
#ifdef WIFI_AUTH_WPA3_PSK
        case WIFI_AUTH_WPA3_PSK:
            return "WPA3";
#endif
#ifdef WIFI_AUTH_WPA2_WPA3_PSK
        case WIFI_AUTH_WPA2_WPA3_PSK:
            return "WPA2/WPA3";
#endif
#ifdef WIFI_AUTH_WAPI_PSK
        case WIFI_AUTH_WAPI_PSK:
            return "WAPI";
#endif
        default:
            return "UNKNOWN";
    }
}

static uint32_t bytes_to_kib(uint64_t bytes) {
    return static_cast<uint32_t>(bytes / 1024ULL);
}

static uint32_t get_partition_used_kib() {
    uint64_t used_bytes = 0;
    esp_partition_iterator_t it = esp_partition_find(
        ESP_PARTITION_TYPE_ANY,
        ESP_PARTITION_SUBTYPE_ANY,
        nullptr);
    while (it != nullptr) {
        const esp_partition_t *partition = esp_partition_get(it);
        if (partition != nullptr) {
            used_bytes += partition->size;
        }
        it = esp_partition_next(it);
    }
    esp_partition_iterator_release(it);
    return bytes_to_kib(used_bytes);
}

static uint32_t get_flash_total_kib() {
    uint32_t flash_size = 0;
    if (esp_flash_get_size(nullptr, &flash_size) != ESP_OK) {
        return 0;
    }
    return bytes_to_kib(flash_size);
}

static const char *wifi_mode_to_string(wifi_mode_t mode) {
    switch (mode) {
        case WIFI_MODE_NULL:
            return "NULL";
        case WIFI_MODE_STA:
            return "STA";
        case WIFI_MODE_AP:
            return "AP";
        case WIFI_MODE_APSTA:
            return "APSTA";
        default:
            return "UNKNOWN";
    }
}

static bool append_camera_status_json(
    char *buffer,
    size_t buffer_size,
    int *offset,
    const CameraI2cStatus *camera) {
    if (camera == nullptr) {
        return false;
    }

    bool ok = append_text(buffer, buffer_size, offset, "{\"reachable\":");
    ok = ok && append_text(buffer, buffer_size, offset, camera->reachable ? "true" : "false");
    ok = ok && append_text(buffer, buffer_size, offset, ",\"valid\":");
    ok = ok && append_text(buffer, buffer_size, offset, camera->valid ? "true" : "false");
    ok = ok && append_format(
        buffer,
        buffer_size,
        offset,
        ",\"last_update_ms\":%lu,\"last_error\":%d,\"error\":",
        static_cast<unsigned long>(camera->last_update_ms),
        static_cast<int>(camera->last_error));
    ok = ok && append_json_string(buffer, buffer_size, offset, esp_err_to_name(camera->last_error));
    ok = ok && append_text(buffer, buffer_size, offset, ",\"ip\":");
    ok = ok && append_json_string(buffer, buffer_size, offset, camera->ip);
    ok = ok && append_text(buffer, buffer_size, offset, ",\"mode\":");
    ok = ok && append_json_string(buffer, buffer_size, offset, camera->mode);
    ok = ok && append_text(buffer, buffer_size, offset, ",\"name\":");
    ok = ok && append_json_string(buffer, buffer_size, offset, camera->name);
    ok = ok && append_text(buffer, buffer_size, offset, ",\"config_url\":");
    ok = ok && append_json_string(buffer, buffer_size, offset, camera->config_url);
    ok = ok && append_text(buffer, buffer_size, offset, ",\"stream_url\":");
    ok = ok && append_json_string(buffer, buffer_size, offset, camera->stream_url);
    return ok && append_text(buffer, buffer_size, offset, "}");
}

static esp_err_t send_provision_page(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");

    const char *cursor = kProvisionHtmlTemplate;
    const char *placeholder = nullptr;
    while ((placeholder = strchr(cursor, '%')) != nullptr) {
        if (placeholder > cursor) {
            esp_err_t err = httpd_resp_send_chunk(
                req, cursor, static_cast<ssize_t>(placeholder - cursor));
            if (err != ESP_OK) {
                return err;
            }
        }

        if (placeholder[1] == 's') {
            esp_err_t err = httpd_resp_send_chunk(req, g_device_name, HTTPD_RESP_USE_STRLEN);
            if (err != ESP_OK) {
                return err;
            }
            cursor = placeholder + 2;
        } else if (placeholder[1] == '%') {
            esp_err_t err = httpd_resp_send_chunk(req, "%", 1);
            if (err != ESP_OK) {
                return err;
            }
            cursor = placeholder + 2;
        } else {
            esp_err_t err = httpd_resp_send_chunk(req, "%", 1);
            if (err != ESP_OK) {
                return err;
            }
            cursor = placeholder + 1;
        }
    }

    if (*cursor != '\0') {
        esp_err_t err = httpd_resp_send_chunk(req, cursor, HTTPD_RESP_USE_STRLEN);
        if (err != ESP_OK) {
            return err;
        }
    }
    return httpd_resp_send_chunk(req, nullptr, 0);
}

static esp_err_t root_get_handler(httpd_req_t *req) {
    return send_provision_page(req);
}

static esp_err_t scan_get_handler(httpd_req_t *req) {
    if (s_http_buffer_mutex == nullptr ||
        xSemaphoreTake(s_http_buffer_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "busy");
        return ESP_FAIL;
    }

    uint16_t ap_count = kMaxScanResults;
    memset(s_scan_records, 0, sizeof(s_scan_records));

    esp_err_t err = esp_wifi_scan_start(NULL, true);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Wi-Fi scan start failed: %s", esp_err_to_name(err));
        xSemaphoreGive(s_http_buffer_mutex);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "scan start failed");
        return ESP_FAIL;
    }

    err = esp_wifi_scan_get_ap_records(&ap_count, s_scan_records);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Wi-Fi scan read failed: %s", esp_err_to_name(err));
        xSemaphoreGive(s_http_buffer_mutex);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "scan read failed");
        return ESP_FAIL;
    }

    memset(s_scan_json, 0, sizeof(s_scan_json));

    int offset = 0;
    bool ok = append_text(s_scan_json, kScanJsonSize, &offset, "{\"device\":");
    ok = ok && append_json_string(s_scan_json, kScanJsonSize, &offset, g_device_name);
    ok = ok && append_text(s_scan_json, kScanJsonSize, &offset, ",\"networks\":[");

    size_t emitted = 0;
    for (uint16_t i = 0; ok && i < ap_count; ++i) {
        if (s_scan_records[i].ssid[0] == '\0') {
            continue;
        }
        ok = ok && append_text(s_scan_json, kScanJsonSize, &offset, (emitted == 0) ? "" : ",");
        ok = ok && append_text(s_scan_json, kScanJsonSize, &offset, "{\"ssid\":");
        ok = ok && append_json_string(s_scan_json, kScanJsonSize, &offset,
                                      reinterpret_cast<const char *>(s_scan_records[i].ssid));
        ok = ok && append_format(s_scan_json, kScanJsonSize, &offset, ",\"rssi\":%d", s_scan_records[i].rssi);
        ok = ok && append_text(s_scan_json, kScanJsonSize, &offset, ",\"auth\":");
        ok = ok && append_json_string(s_scan_json, kScanJsonSize, &offset,
                                      auth_mode_to_string(s_scan_records[i].authmode));
        ok = ok && append_format(s_scan_json, kScanJsonSize, &offset, ",\"open\":%s}",
                                 (s_scan_records[i].authmode == WIFI_AUTH_OPEN) ? "true" : "false");
        if (ok) {
            ++emitted;
        }
    }

    ok = ok && append_text(s_scan_json, kScanJsonSize, &offset, "]}");
    if (!ok) {
        xSemaphoreGive(s_http_buffer_mutex);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "scan payload too large");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    const esp_err_t send_err = httpd_resp_send(req, s_scan_json, HTTPD_RESP_USE_STRLEN);
    xSemaphoreGive(s_http_buffer_mutex);
    return send_err;
}

static esp_err_t status_get_handler(httpd_req_t *req) {
    if (s_http_buffer_mutex == nullptr ||
        xSemaphoreTake(s_http_buffer_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "busy");
        return ESP_FAIL;
    }
    memset(s_status_json, 0, sizeof(s_status_json));

    wifi_mode_t wifi_mode = WIFI_MODE_NULL;
    esp_wifi_get_mode(&wifi_mode);

    wifi_ap_record_t ap_info = {};
    const bool sta_connected = (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK);

    esp_netif_ip_info_t ap_ip_info = {};
    const bool has_ap_ip = (wifi_get_softap_ip_info(&ap_ip_info) == ESP_OK);

    ImuMsg imu = {};
    MotionMsg motion = {};
    UltrasonicMsg ultrasonic = {};
    GamepadMsg gamepad = {};
    TemperatureMsg temperature = {};
    BatteryMsg battery = {};
    LidarMsg lidar = {};

    const bool has_imu = q_imu_state != nullptr && xQueuePeek(q_imu_state, &imu, 0) == pdTRUE;
    const bool has_motion = q_motion_state != nullptr && xQueuePeek(q_motion_state, &motion, 0) == pdTRUE;
    const bool has_ultrasonic = q_ultrasonic_state != nullptr && xQueuePeek(q_ultrasonic_state, &ultrasonic, 0) == pdTRUE;
    const bool has_gamepad = q_gamepad_state != nullptr && xQueuePeek(q_gamepad_state, &gamepad, 0) == pdTRUE;
    const bool has_temperature = q_temperature_state != nullptr &&
                                 xQueuePeek(q_temperature_state, &temperature, 0) == pdTRUE &&
                                 temperature.valid;
    const bool has_battery = q_battery_state != nullptr &&
                             xQueuePeek(q_battery_state, &battery, 0) == pdTRUE &&
                             battery.valid;
    const bool has_lidar = q_lidar_state != nullptr && xQueuePeek(q_lidar_state, &lidar, 0) == pdTRUE;

    CameraI2cStatus camera = {};
    esp_err_t camera_err = camera_i2c_client_get_cached_status(&camera);
    if (camera_err != ESP_OK) {
        camera.valid = false;
        camera.reachable = false;
        camera.last_error = camera_err;
    }

    uint16_t lidar_valid_points = 0;
    uint16_t lidar_min_mm = UINT16_MAX;
    uint16_t lidar_max_mm = 0;
    if (has_lidar) {
        for (size_t i = 0; i < sizeof(lidar.distances) / sizeof(lidar.distances[0]); ++i) {
            const uint16_t distance = lidar.distances[i];
            if (distance == 0) {
                continue;
            }
            ++lidar_valid_points;
            if (distance < lidar_min_mm) {
                lidar_min_mm = distance;
            }
            if (distance > lidar_max_mm) {
                lidar_max_mm = distance;
            }
        }
    }
    if (lidar_valid_points == 0) {
        lidar_min_mm = 0;
    }

    const size_t heap_total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    const size_t heap_free = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    const size_t heap_used = heap_total > heap_free ? heap_total - heap_free : 0;
    const uint32_t flash_total_kib = get_flash_total_kib();
    const uint32_t flash_used_kib = get_partition_used_kib();

    int offset = 0;
    bool ok = append_text(s_status_json, kStatusJsonSize, &offset, "{\"device_name\":");
    ok = ok && append_json_string(s_status_json, kStatusJsonSize, &offset, g_device_name);
    ok = ok && append_format(
        s_status_json,
        kStatusJsonSize,
        &offset,
        ",\"uptime_ms\":%lu,\"emergency_stop\":%s,\"motion_busy\":%s,\"wifi_comm_mode\":",
        static_cast<unsigned long>(xTaskGetTickCount() * portTICK_PERIOD_MS),
        g_emergency_stop ? "true" : "false",
        g_motion_busy ? "true" : "false");
    ok = ok && append_json_string(
        s_status_json,
        kStatusJsonSize,
        &offset,
        g_wifi_comm_mode == WifiCommMode::kMicroRos ? "micro_ros" : "mavlink_udp");
    ok = ok && append_text(s_status_json, kStatusJsonSize, &offset, ",\"runtime\":{\"comm_mode\":");
    ok = ok && append_json_string(
        s_status_json,
        kStatusJsonSize,
        &offset,
        g_wifi_comm_mode == WifiCommMode::kMicroRos ? "micro_ros" : "mavlink_udp");
    ok = ok && append_text(s_status_json, kStatusJsonSize, &offset, ",\"microros_agent_ip\":");
    ok = ok && append_json_string(s_status_json, kStatusJsonSize, &offset, g_microros_agent_ip);
    ok = ok && append_format(
        s_status_json,
        kStatusJsonSize,
        &offset,
        ",\"microros_agent_port\":%u}",
        static_cast<unsigned>(g_microros_agent_port));

    ok = ok && append_text(s_status_json, kStatusJsonSize, &offset, ",\"wifi\":{\"mode\":");
    ok = ok && append_json_string(s_status_json, kStatusJsonSize, &offset, wifi_mode_to_string(wifi_mode));
    ok = ok && append_format(s_status_json, kStatusJsonSize, &offset, ",\"sta_connected\":%s", sta_connected ? "true" : "false");
    if (sta_connected) {
        ok = ok && append_text(s_status_json, kStatusJsonSize, &offset, ",\"ssid\":");
        ok = ok && append_json_string(s_status_json, kStatusJsonSize, &offset, reinterpret_cast<const char *>(ap_info.ssid));
        ok = ok && append_format(s_status_json, kStatusJsonSize, &offset, ",\"rssi\":%d,\"channel\":%u", ap_info.rssi, ap_info.primary);
    }
    if (has_ap_ip) {
        ok = ok && append_format(
            s_status_json,
            kStatusJsonSize,
            &offset,
            ",\"ap_ip\":\"" IPSTR "\"",
            IP2STR(&ap_ip_info.ip));
    }
    ok = ok && append_text(s_status_json, kStatusJsonSize, &offset, "}");

    ok = ok && append_text(s_status_json, kStatusJsonSize, &offset, ",\"camera\":");
    ok = ok && append_camera_status_json(s_status_json, kStatusJsonSize, &offset, &camera);

    ok = ok && append_format(
        s_status_json,
        kStatusJsonSize,
        &offset,
        ",\"memory\":{\"heap_total_kib\":%lu,\"heap_free_kib\":%lu,\"heap_used_kib\":%lu,\"heap_min_free_kib\":%lu}",
        static_cast<unsigned long>(bytes_to_kib(heap_total)),
        static_cast<unsigned long>(bytes_to_kib(heap_free)),
        static_cast<unsigned long>(bytes_to_kib(heap_used)),
        static_cast<unsigned long>(bytes_to_kib(heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT))));

    ok = ok && append_format(
        s_status_json,
        kStatusJsonSize,
        &offset,
        ",\"storage\":{\"flash_total_kib\":%lu,\"partition_used_kib\":%lu}",
        static_cast<unsigned long>(flash_total_kib),
        static_cast<unsigned long>(flash_used_kib));

    ok = ok && append_format(
        s_status_json,
        kStatusJsonSize,
        &offset,
        ",\"temperature\":{\"valid\":%s,\"celsius\":%.2f}",
        has_temperature ? "true" : "false",
        has_temperature ? temperature.celsius : 0.0f);

    ok = ok && append_format(
        s_status_json,
        kStatusJsonSize,
        &offset,
        ",\"battery\":{\"valid\":%s,\"voltage_v\":%.3f,\"adc_voltage_v\":%.4f,\"percentage\":%u,\"raw\":%d}",
        has_battery ? "true" : "false",
        has_battery ? battery.voltage_v : 0.0f,
        has_battery ? battery.adc_voltage_v : 0.0f,
        has_battery ? battery.percentage : 0,
        has_battery ? battery.raw : 0);

    ok = ok && append_format(
        s_status_json,
        kStatusJsonSize,
        &offset,
        ",\"imu\":{\"valid\":%s,\"roll\":%.2f,\"pitch\":%.2f,\"yaw\":%.2f,\"acc_x\":%.3f,\"acc_y\":%.3f,\"acc_z\":%.3f,\"gyro_x\":%.3f,\"gyro_y\":%.3f,\"gyro_z\":%.3f}",
        has_imu ? "true" : "false",
        imu.roll,
        imu.pitch,
        imu.yaw,
        imu.acc_x,
        imu.acc_y,
        imu.acc_z,
        imu.gyro_x,
        imu.gyro_y,
        imu.gyro_z);

    ok = ok && append_format(
        s_status_json,
        kStatusJsonSize,
        &offset,
        ",\"motion\":{\"valid\":%s,\"source\":%u,\"control_mode\":%u,\"vx\":%.2f,\"vy\":%.2f,\"wz\":%.3f,\"x\":%.2f,\"y\":%.2f,\"yaw\":%.2f,\"target_vx\":%.2f,\"target_vy\":%.2f,\"target_wz\":%.3f,\"target_x\":%.2f,\"target_y\":%.2f,\"target_yaw\":%.2f,\"vel_left\":%.2f,\"vel_right\":%.2f}",
        has_motion ? "true" : "false",
        motion.source,
        motion.control_mode,
        motion.vx,
        motion.vy,
        motion.wz,
        motion.x,
        motion.y,
        motion.yaw,
        motion.target_vx,
        motion.target_vy,
        motion.target_wz,
        motion.target_x,
        motion.target_y,
        motion.target_yaw,
        motion.vel_left,
        motion.vel_right);

    ok = ok && append_format(
        s_status_json,
        kStatusJsonSize,
        &offset,
        ",\"ultrasonic\":{\"valid\":%s,\"distance_cm\":%.2f,\"obstacle\":%s}",
        has_ultrasonic ? "true" : "false",
        ultrasonic.distance_cm,
        ultrasonic.is_obstacle_detected ? "true" : "false");

    ok = ok && append_format(
        s_status_json,
        kStatusJsonSize,
        &offset,
        ",\"gamepad\":{\"valid\":%s,\"connected\":%s,\"left_x\":%d,\"left_y\":%d,\"right_x\":%d,\"right_y\":%d,\"buttons\":%u,\"button_mask\":%u,\"update_ms\":%lu}",
        has_gamepad ? "true" : "false",
        gamepad.connected ? "true" : "false",
        gamepad.left_x,
        gamepad.left_y,
        gamepad.right_x,
        gamepad.right_y,
        gamepad.buttons,
        gamepad.button_mask,
        static_cast<unsigned long>(gamepad.update_ms));

    ok = ok && append_format(
        s_status_json,
        kStatusJsonSize,
        &offset,
        ",\"lidar\":{\"valid\":%s,\"valid_points\":%u,\"min_mm\":%u,\"max_mm\":%u}",
        has_lidar ? "true" : "false",
        lidar_valid_points,
        lidar_min_mm,
        lidar_max_mm);

    ok = ok && append_format(
        s_status_json,
        kStatusJsonSize,
        &offset,
        ",\"pid\":{\"speed\":{\"kp\":%.4f,\"ki\":%.4f,\"kd\":%.4f},\"position\":{\"kp\":%.4f,\"ki\":%.4f,\"kd\":%.4f}}",
        g_speed_pid_state.kp,
        g_speed_pid_state.ki,
        g_speed_pid_state.kd,
        g_position_pid_state.kp,
        g_position_pid_state.ki,
        g_position_pid_state.kd);

    ok = ok && append_text(s_status_json, kStatusJsonSize, &offset, "}");
    if (!ok) {
        xSemaphoreGive(s_http_buffer_mutex);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "status payload too large");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    const esp_err_t send_err = httpd_resp_send(req, s_status_json, HTTPD_RESP_USE_STRLEN);
    xSemaphoreGive(s_http_buffer_mutex);
    return send_err;
}

static esp_err_t provision_post_handler(httpd_req_t *req) {
    if (req->content_len <= 0 || req->content_len >= 256) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "invalid payload");
        return ESP_FAIL;
    }

    char body[256] = {0};
    const int received = httpd_req_recv(req, body, req->content_len);
    if (received <= 0) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "read failed");
        return ESP_FAIL;
    }
    body[received] = '\0';

    char ssid[33] = {0};
    char password[65] = {0};
    char lang[8] = {0};
    extract_form_field(body, "ssid", ssid, sizeof(ssid));
    extract_form_field(body, "password", password, sizeof(password));
    extract_form_field(body, "lang", lang, sizeof(lang));

    if (ssid[0] == '\0') {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "ssid required");
        return ESP_FAIL;
    }
    if (password[0] != '\0' && strlen(password) < 8) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "password too short");
        return ESP_FAIL;
    }

    esp_err_t err = wifi_save_sta_credentials(ssid, password);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save Wi-Fi credentials: %s", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "save failed");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Saved Wi-Fi credentials for SSID: %s", ssid);
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    if (is_lang_en(lang)) {
        httpd_resp_sendstr(
            req,
            "<html><body style='font-family:Segoe UI,sans-serif;padding:24px;'>"
            "<h2>Wi-Fi saved</h2>"
            "<p>The device is restarting now. Reconnect to the target Wi-Fi and then use the device normally.</p>"
            "</body></html>");
    } else {
        httpd_resp_sendstr(
            req,
            "<html><body style='font-family:Segoe UI,PingFang SC,sans-serif;padding:24px;'>"
            "<h2>Wi-Fi 已保存</h2>"
            "<p>设备正在重启，请切换回目标 Wi-Fi，随后即可正常使用设备。</p>"
            "</body></html>");
    }

    xTaskCreate(restart_after_delay_task, "wifi_restart", 2048, NULL, 3, NULL);
    return ESP_OK;
}

static esp_err_t camera_provision_post_handler(httpd_req_t *req) {
    if (req->content_len <= 0 || req->content_len >= 256) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "invalid payload");
        return ESP_FAIL;
    }

    char body[256] = {0};
    const int received = httpd_req_recv(req, body, req->content_len);
    if (received <= 0) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "read failed");
        return ESP_FAIL;
    }
    body[received] = '\0';

    char ssid[33] = {0};
    char password[65] = {0};
    char format_value[16] = {0};
    extract_form_field(body, "ssid", ssid, sizeof(ssid));
    extract_form_field(body, "password", password, sizeof(password));
    extract_form_field(body, "format", format_value, sizeof(format_value));

    if (ssid[0] == '\0') {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "ssid required");
        return ESP_FAIL;
    }
    if (password[0] != '\0' && strlen(password) < 8) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "password too short");
        return ESP_FAIL;
    }

    camera_i2c_provision_format_t format = CAMERA_I2C_PROVISION_WIFI;
    if (format_value[0] != '\0' && !parse_camera_provision_format(format_value, &format)) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "invalid camera provision format");
        return ESP_FAIL;
    }

    CameraI2cStatus camera = {};
    esp_err_t err = camera_i2c_client_provision_wifi_and_read(ssid, password, format, &camera);
    if (err != ESP_OK && err != ESP_ERR_TIMEOUT && !camera.reachable) {
        ESP_LOGW(TAG, "Camera I2C provisioning failed: %s", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, esp_err_to_name(err));
        return ESP_FAIL;
    }

    char response_json[1024] = {};
    int offset = 0;
    bool ok = append_text(response_json, sizeof(response_json), &offset, "{\"ok\":true,\"camera\":");
    ok = ok && append_camera_status_json(response_json, sizeof(response_json), &offset, &camera);
    ok = ok && append_text(response_json, sizeof(response_json), &offset, "}");
    if (!ok) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "camera payload too large");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, response_json, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t runtime_config_post_handler(httpd_req_t *req) {
    if (req->content_len <= 0 || req->content_len >= 256) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "invalid payload");
        return ESP_FAIL;
    }

    char body[256] = {0};
    const int received = httpd_req_recv(req, body, req->content_len);
    if (received <= 0) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "read failed");
        return ESP_FAIL;
    }
    body[received] = '\0';

    char comm_mode_value[16] = {0};
    char agent_ip[16] = {0};
    char agent_port_value[8] = {0};
    extract_form_field(body, "comm_mode", comm_mode_value, sizeof(comm_mode_value));
    extract_form_field(body, "microros_agent_ip", agent_ip, sizeof(agent_ip));
    extract_form_field(body, "microros_agent_port", agent_port_value, sizeof(agent_port_value));

    WifiRuntimeConfig config = {};
    if (!parse_comm_mode(comm_mode_value, &config.comm_mode)) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "invalid comm mode");
        return ESP_FAIL;
    }
    if (!is_valid_ipv4_address(agent_ip)) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "invalid micro-ROS agent IP");
        return ESP_FAIL;
    }

    char *port_end = nullptr;
    const long port = strtol(agent_port_value, &port_end, 10);
    if (agent_port_value[0] == '\0' || port_end == agent_port_value ||
        *port_end != '\0' || port <= 0 || port > 65535) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "invalid micro-ROS agent port");
        return ESP_FAIL;
    }

    strncpy(config.microros_agent_ip, agent_ip, sizeof(config.microros_agent_ip) - 1);
    config.microros_agent_ip[sizeof(config.microros_agent_ip) - 1] = '\0';
    config.microros_agent_port = static_cast<uint16_t>(port);

    esp_err_t err = wifi_save_runtime_config(&config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save runtime config: %s", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "save failed");
        return ESP_FAIL;
    }

    g_wifi_comm_mode = config.comm_mode;
    snprintf(g_microros_agent_ip, sizeof(g_microros_agent_ip), "%s",
             config.microros_agent_ip);
    g_microros_agent_port = config.microros_agent_port;

    ESP_LOGI(TAG, "Saved runtime config: comm=%s, micro-ROS agent=%s:%u",
             g_wifi_comm_mode == WifiCommMode::kMicroRos ? "micro_ros" : "mavlink_udp",
             g_microros_agent_ip,
             static_cast<unsigned>(g_microros_agent_port));
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"ok\":true,\"restart\":true}");
    xTaskCreate(restart_after_delay_task, "runtime_restart", 2048, NULL, 3, NULL);
    return ESP_OK;
}

static int find_dns_question_end(const uint8_t *packet, int packet_len) {
    int index = 12;
    while (index < packet_len) {
        const uint8_t label_len = packet[index++];
        if (label_len == 0) {
            break;
        }
        index += label_len;
    }

    if (index + 4 > packet_len) {
        return -1;
    }
    return index + 4;
}

static void captive_dns_task(void *pvParameters) {
    esp_netif_ip_info_t ap_ip_info = {};
    if (wifi_get_softap_ip_info(&ap_ip_info) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SoftAP IP info for captive DNS");
        vTaskDelete(NULL);
        return;
    }

    const uint32_t ap_ip = ap_ip_info.ip.addr;
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Failed to create captive DNS socket");
        vTaskDelete(NULL);
        return;
    }

    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(53);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) != 0) {
        ESP_LOGE(TAG, "Failed to bind captive DNS socket");
        close(sock);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "Captive DNS started on UDP/53");

    uint8_t request[kDnsPacketMaxSize] = {0};
    uint8_t response[kDnsPacketMaxSize] = {0};

    while (true) {
        struct sockaddr_in client_addr = {};
        socklen_t client_len = sizeof(client_addr);
        const int recv_len = recvfrom(sock, request, sizeof(request), 0,
                                      reinterpret_cast<struct sockaddr *>(&client_addr), &client_len);
        if (recv_len < 12) {
            continue;
        }

        const int question_end = find_dns_question_end(request, recv_len);
        if (question_end <= 0 || question_end > recv_len) {
            continue;
        }

        memcpy(response, request, question_end);
        response[2] = 0x81;
        response[3] = 0x80;
        response[4] = 0x00;
        response[5] = 0x01;
        response[8] = 0x00;
        response[9] = 0x00;
        response[10] = 0x00;
        response[11] = 0x00;

        const uint16_t query_type =
            static_cast<uint16_t>((request[question_end - 4] << 8) | request[question_end - 3]);
        const uint16_t query_class =
            static_cast<uint16_t>((request[question_end - 2] << 8) | request[question_end - 1]);

        int response_len = question_end;
        if (query_type == 0x0001 && query_class == 0x0001 &&
            question_end + 16 <= static_cast<int>(sizeof(response))) {
            response[6] = 0x00;
            response[7] = 0x01;

            response[response_len++] = 0xC0;
            response[response_len++] = 0x0C;
            response[response_len++] = 0x00;
            response[response_len++] = 0x01;
            response[response_len++] = 0x00;
            response[response_len++] = 0x01;
            response[response_len++] = 0x00;
            response[response_len++] = 0x00;
            response[response_len++] = 0x00;
            response[response_len++] = 0x3C;
            response[response_len++] = 0x00;
            response[response_len++] = 0x04;
            memcpy(response + response_len, &ap_ip, 4);
            response_len += 4;
        } else {
            response[6] = 0x00;
            response[7] = 0x00;
        }

        sendto(sock, response, response_len, 0,
               reinterpret_cast<struct sockaddr *>(&client_addr), client_len);
    }
}

void wifi_provision_task(void *pvParameters) {
    if (s_http_buffer_mutex == nullptr) {
        s_http_buffer_mutex = xSemaphoreCreateMutex();
        if (s_http_buffer_mutex == nullptr) {
            ESP_LOGE(TAG, "Failed to create HTTP buffer mutex");
            vTaskDelete(NULL);
            return;
        }
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.stack_size = 12288;
    config.max_uri_handlers = 13;
    config.uri_match_fn = httpd_uri_match_wildcard;

    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start provisioning HTTP server");
        vTaskDelete(NULL);
        return;
    }

    httpd_uri_t uri_root = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_get_handler,
        .user_ctx = NULL,
    };
    httpd_uri_t uri_scan = {
        .uri = "/api/scan",
        .method = HTTP_GET,
        .handler = scan_get_handler,
        .user_ctx = NULL,
    };
    httpd_uri_t uri_status = {
        .uri = "/api/status",
        .method = HTTP_GET,
        .handler = status_get_handler,
        .user_ctx = NULL,
    };
    httpd_uri_t uri_provision = {
        .uri = "/api/provision",
        .method = HTTP_POST,
        .handler = provision_post_handler,
        .user_ctx = NULL,
    };
    httpd_uri_t uri_runtime_config = {
        .uri = "/api/runtime-config",
        .method = HTTP_POST,
        .handler = runtime_config_post_handler,
        .user_ctx = NULL,
    };
    httpd_uri_t uri_camera_provision = {
        .uri = "/api/camera/provision",
        .method = HTTP_POST,
        .handler = camera_provision_post_handler,
        .user_ctx = NULL,
    };
    httpd_uri_t uri_captive_all = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = root_get_handler,
        .user_ctx = NULL,
    };

    httpd_register_uri_handler(server, &uri_root);
    httpd_register_uri_handler(server, &uri_scan);
    httpd_register_uri_handler(server, &uri_status);
    httpd_register_uri_handler(server, &uri_provision);
    httpd_register_uri_handler(server, &uri_runtime_config);
    httpd_register_uri_handler(server, &uri_camera_provision);
    httpd_register_uri_handler(server, &uri_captive_all);

    xTaskCreate(captive_dns_task, "captive_dns", 4096, NULL, 3, NULL);

    ESP_LOGI(TAG, "Provisioning server started on port 80");
    vTaskDelete(NULL);
}
