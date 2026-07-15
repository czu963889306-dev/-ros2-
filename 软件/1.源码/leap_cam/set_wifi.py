from __future__ import annotations

import argparse
import sys
from dataclasses import dataclass
from http.cookiejar import CookieJar
from typing import Union
from urllib.error import HTTPError, URLError
from urllib.parse import urlencode
from urllib.request import (
    HTTPCookieProcessor,
    Request,
    build_opener,
)

DEFAULT_HOST = "192.168.1.88"
DEFAULT_PORT = 80

DEFAULT_WIFI_SSID = "czulcl"
DEFAULT_WIFI_PASSWORD = "qwer1234"

DEFAULT_CGI_PATH = "/web/cgi-bin/his3510/param.cgi"


@dataclass(frozen=True)
class CommandResult:
    command: str
    status: int
    reason: str
    body: str


@dataclass(frozen=True)
class CommandSpec:
    command: str
    refer_page: str
    fields: list[tuple[str, str]]
    include_cururl: bool = False


@dataclass(frozen=True)
class WifiConfig:
    ssid: str = DEFAULT_WIFI_SSID
    password: str = DEFAULT_WIFI_PASSWORD
    auth: str = "3"
    mode: str = "0"
    enc: str = "0"
    enable: str = "1"


@dataclass(frozen=True)
class VideoStreamConfig:
    chn: str
    bps: str
    fps: str
    brmode: str = "1"
    imagegrade: str = "1"
    gop: str = "160"


@dataclass(frozen=True)
class VideoConfig:
    videomode: str
    vinorm: str
    profile: str
    main_stream: VideoStreamConfig
    sub_stream: VideoStreamConfig


@dataclass(frozen=True)
class OSDConfig:
    time_show: str = "1"
    time_place: str = "1"
    name_show: str = "1"
    name: str = "IPCAM"
    name_place: str = "0"


@dataclass(frozen=True)
class HumanoidDetectionConfig:
    switch: str
    smd_enable: str
    smd_rect: str
    smd_gthresh: str


@dataclass(frozen=True)
class ONVIFConfig:
    ov_enable: str
    ov_port: str
    ov_authflag: str
    ov_forbitset: str
    ov_subchn: str
    ov_snapchn: str
    ov_nvctype: str


@dataclass(frozen=True)
class P2PConfig:
    xqp2p_enable: str


IPCamConfig = Union[
    WifiConfig,
    VideoConfig,
    OSDConfig,
    HumanoidDetectionConfig,
    ONVIFConfig,
    P2PConfig,
]


class IPCamClient:
    def __init__(
        self,
        host: str = DEFAULT_HOST,
        port: int = DEFAULT_PORT,
        cgi_path: str = DEFAULT_CGI_PATH,
        timeout: float = 10.0,
    ) -> None:
        self.host = host
        self.port = port
        self.cgi_path = cgi_path
        self.timeout = timeout
        self.base_url = self._build_base_url()
        self.post_url = f"{self.base_url}{self.cgi_path}"
        self.opener = build_opener(HTTPCookieProcessor(CookieJar()))

    def _build_base_url(self) -> str:
        if self.port == 80:
            return f"http://{self.host}"
        return f"http://{self.host}:{self.port}"

    def _build_referer_url(self, refer_page: str) -> str:
        return f"{self.base_url}{refer_page}"

    def open_page(self, refer_page: str) -> None:
        referer_url = self._build_referer_url(refer_page)
        request = Request(
            url=referer_url,
            method="GET",
            headers={
                "User-Agent": "python-ipcam-config/1.0",
            },
        )

        with self.opener.open(request, timeout=self.timeout) as response:
            response.read()

    def _build_form_data(self, spec: CommandSpec) -> list[tuple[str, str]]:
        form_data: list[tuple[str, str]] = [("cmd", spec.command)]

        # his3510 网页实际字段是 cururl，不是 curl。
        if spec.include_cururl:
            form_data.append(("cururl", self._build_referer_url(spec.refer_page)))

        form_data.extend(spec.fields)
        return form_data

    def _send_form(
        self,
        form_data: list[tuple[str, str]],
        refer_page: str,
    ) -> CommandResult:
        referer_url = self._build_referer_url(refer_page)
        payload = urlencode(form_data).encode("ascii")

        request = Request(
            url=self.post_url,
            data=payload,
            method="POST",
            headers={
                "Content-Type": "application/x-www-form-urlencoded",
                "Origin": self.base_url,
                "Referer": referer_url,
                "User-Agent": "python-ipcam-config/1.0",
            },
        )

        with self.opener.open(request, timeout=self.timeout) as response:
            body = response.read().decode("utf-8", errors="replace")
            first_command = next(value for key, value in form_data if key == "cmd")

            return CommandResult(
                command=first_command,
                status=response.status,
                reason=response.reason,
                body=body,
            )

    def _execute_specs(
        self,
        specs: list[CommandSpec],
        login_page: str,
    ) -> list[CommandResult]:
        self.open_page(login_page)

        return [
            self._send_form(self._build_form_data(spec), spec.refer_page)
            for spec in specs
        ]

    def preview_forms(self, config: IPCamConfig) -> list[list[tuple[str, str]]]:
        specs, _login_page = build_command_specs(config)
        return [self._build_form_data(spec) for spec in specs]

    def apply_config(self, config: IPCamConfig) -> list[CommandResult]:
        validate_config(config)
        specs, login_page = build_command_specs(config)
        return self._execute_specs(specs, login_page)

    def set_wifi(self, config: WifiConfig) -> list[CommandResult]:
        return self.apply_config(config)

    def set_video(self, config: VideoConfig) -> list[CommandResult]:
        return self.apply_config(config)

    def set_osd(self, config: OSDConfig) -> list[CommandResult]:
        return self.apply_config(config)

    def set_humanoid_detection(self, config: HumanoidDetectionConfig) -> list[CommandResult]:
        return self.apply_config(config)

    def set_onvif(self, config: ONVIFConfig) -> list[CommandResult]:
        return self.apply_config(config)

    def set_p2p(self, config: P2PConfig) -> list[CommandResult]:
        return self.apply_config(config)


def build_command_specs(config: IPCamConfig) -> tuple[list[CommandSpec], str]:
    if isinstance(config, WifiConfig):
        return [
            CommandSpec(
                command="setwirelessattr",
                refer_page="/web/wifi.html",
                include_cururl=True,
                fields=[
                    ("-wf_ssid", config.ssid),
                    ("-wf_auth", config.auth),
                    ("-wf_mode", config.mode),
                    ("-wf_enc", config.enc),
                    ("-wf_enable", config.enable),
                    ("-wf_key", config.password),
                ],
            )
        ], "/web/wifi.html"

    if isinstance(config, VideoConfig):
        return [
            CommandSpec(
                command="setvideoattr",
                refer_page="/web/video.html",
                include_cururl=True,
                fields=[
                    ("-videomode", "41"),
                    ("-vinorm", "P"),
                    ("-profile", config.profile),

                    ("cmd", "setvencattr"),
                    ("-chn", "11"),
                    ("-bps", config.main_stream.bps),
                    ("-fps", config.main_stream.fps),
                    ("-brmode", config.main_stream.brmode),
                    ("-imagegrade", config.main_stream.imagegrade),
                    ("-gop", config.main_stream.gop),

                    ("cmd", "setvencattr"),
                    ("-chn", "12"),
                    ("-bps", config.sub_stream.bps),
                    ("-fps", config.sub_stream.fps),
                    ("-brmode", config.sub_stream.brmode),
                    ("-imagegrade", config.sub_stream.imagegrade),
                    ("-gop", config.sub_stream.gop),
                ],
            )
        ], "/web/video.html"

    if isinstance(config, OSDConfig):
        return [
            CommandSpec(
                command="setoverlayattr",
                refer_page="/web/osd.html",
                include_cururl=True,
                fields=[
                    ("-region", "0"),
                    ("-show", config.time_show),
                    ("-place", config.time_place),

                    ("cmd", "setoverlayattr"),
                    ("-region", "1"),
                    ("-show", config.name_show),
                    ("-name", config.name),
                    ("-place", config.name_place),
                ],
            )
        ], "/web/osd.html"

    if isinstance(config, HumanoidDetectionConfig):
        return [
            CommandSpec(
                command="setmdalarm",
                refer_page="/web/alarmsmd.html",
                fields=[
                    ("-aname", "type"),
                    ("-switch", config.switch),
                ],
            ),
            CommandSpec(
                command="setsmdattr",
                refer_page="/web/alarmsmd.html",
                include_cururl=True,
                fields=[
                    ("-smd_enable", config.smd_enable),
                ],
            ),
            CommandSpec(
                command="setsmdex",
                refer_page="/web/alarmsmd.html",
                fields=[
                    ("-smd_rect", config.smd_rect),
                    ("-smd_gthresh", config.smd_gthresh),
                ],
            ),
        ], "/web/alarmsmd.html"

    if isinstance(config, ONVIFConfig):
        return [
            CommandSpec(
                command="setonvifattr",
                refer_page="/web/onvif.html",
                include_cururl=True,
                fields=[
                    ("-ov_enable", config.ov_enable),
                    ("-ov_port", config.ov_port),
                    ("-ov_authflag", config.ov_authflag),
                    ("-ov_forbitset", config.ov_forbitset),
                    ("-ov_subchn", config.ov_subchn),
                    ("-ov_snapchn", config.ov_snapchn),
                    ("-ov_nvctype", config.ov_nvctype),
                ],
            )
        ], "/web/onvif.html"

    if isinstance(config, P2PConfig):
        return [
            CommandSpec(
                command="setxqp2pattr",
                refer_page="/web/xqplatform.html",
                include_cururl=True,
                fields=[
                    ("-xqp2p_enable", config.xqp2p_enable),
                ],
            )
        ], "/web/xqplatform.html"

    raise TypeError(f"Unsupported config type: {type(config).__name__}")


def validate_binary_flag(value: str, name: str) -> None:
    if value not in {"0", "1"}:
        raise ValueError(f"{name} must be '0' or '1'.")


def validate_config(config: IPCamConfig) -> None:
    if isinstance(config, WifiConfig):
        if not config.ssid:
            raise ValueError("SSID must not be empty.")
        if not config.password:
            raise ValueError("WiFi password must not be empty.")
        validate_binary_flag(config.enable, "WiFi enable")

    elif isinstance(config, OSDConfig):
        validate_binary_flag(config.time_show, "OSD time_show")
        validate_binary_flag(config.name_show, "OSD name_show")

    elif isinstance(config, HumanoidDetectionConfig):
        validate_binary_flag(config.switch, "Humanoid switch")
        validate_binary_flag(config.smd_enable, "Humanoid smd_enable")
        if not config.smd_rect:
            raise ValueError("Humanoid smd_rect must not be empty.")
        if not config.smd_gthresh:
            raise ValueError("Humanoid smd_gthresh must not be empty.")

    elif isinstance(config, ONVIFConfig):
        validate_binary_flag(config.ov_enable, "ONVIF ov_enable")
        if not config.ov_port:
            raise ValueError("ONVIF ov_port must not be empty.")

    elif isinstance(config, P2PConfig):
        validate_binary_flag(config.xqp2p_enable, "P2P xqp2p_enable")


def mask_form_data(form_data: list[tuple[str, str]]) -> list[tuple[str, str]]:
    secret_keys = {"-wf_key"}

    return [
        (key, "*" * len(value) if key in secret_keys else value)
        for key, value in form_data
    ]


def print_preview(client: IPCamClient, config: IPCamConfig) -> None:
    print(f"POST URL: {client.post_url}")

    for index, form_data in enumerate(client.preview_forms(config), start=1):
        print(f"Form data #{index}: {mask_form_data(form_data)}")


def add_wifi_parser(subparsers: argparse._SubParsersAction) -> None:
    parser = subparsers.add_parser("wifi", help="Configure WiFi")
    parser.add_argument("--ssid", default=DEFAULT_WIFI_SSID, help="Target WiFi SSID")
    parser.add_argument("--password", default=DEFAULT_WIFI_PASSWORD, help="Target WiFi password")
    parser.add_argument("--auth", default="3", help="Value for -wf_auth")
    parser.add_argument("--mode", default="0", help="Value for -wf_mode")
    parser.add_argument("--enc", default="0", help="Value for -wf_enc")
    parser.add_argument("--enable", choices=("0", "1"), default="1", help="Value for -wf_enable")


def add_video_parser(subparsers: argparse._SubParsersAction) -> None:
    parser = subparsers.add_parser("video", help="Configure video attributes and streams")
    parser.add_argument("--videomode", help="Value for -videomode")
    parser.add_argument("--vinorm", help="Value for -vinorm")
    parser.add_argument("--profile", required=True, help="Value for -profile")

    parser.add_argument("--main-chn", help="Main stream value for -chn")
    parser.add_argument("--main-bps", required=True, help="Main stream value for -bps")
    parser.add_argument("--main-fps", required=True, help="Main stream value for -fps")
    parser.add_argument("--main-brmode", default="1", help="Main stream value for -brmode")
    parser.add_argument("--main-imagegrade", default="1", help="Main stream value for -imagegrade")
    parser.add_argument("--main-gop", default="160", help="Main stream value for -gop")

    parser.add_argument("--sub-chn", help="Sub stream value for -chn")
    parser.add_argument("--sub-bps", required=True, help="Sub stream value for -bps")
    parser.add_argument("--sub-fps", required=True, help="Sub stream value for -fps")
    parser.add_argument("--sub-brmode", default="1", help="Sub stream value for -brmode")
    parser.add_argument("--sub-imagegrade", default="1", help="Sub stream value for -imagegrade")
    parser.add_argument("--sub-gop", default="160", help="Sub stream value for -gop")


def add_osd_parser(subparsers: argparse._SubParsersAction) -> None:
    parser = subparsers.add_parser("osd", help="Configure OSD overlay")
    parser.add_argument("--time-show", choices=("0", "1"), default="1", help="Show time overlay")
    parser.add_argument("--time-place", default="1", help="Time overlay place")
    parser.add_argument("--name-show", choices=("0", "1"), default="1", help="Show name overlay")
    parser.add_argument("--name", default="IPCAM", help="OSD display name")
    parser.add_argument("--name-place", default="0", help="Name overlay place")


def add_humanoid_parser(subparsers: argparse._SubParsersAction) -> None:
    parser = subparsers.add_parser("humanoid", help="Configure humanoid detection")
    parser.add_argument("--switch", choices=("0", "1"), required=True, help="Value for setmdalarm -switch")
    parser.add_argument("--smd-enable", choices=("0", "1"), required=True, help="Value for -smd_enable")
    parser.add_argument("--smd-rect", required=True, help="Value for -smd_rect")
    parser.add_argument("--smd-gthresh", required=True, help="Value for -smd_gthresh")


def add_onvif_parser(subparsers: argparse._SubParsersAction) -> None:
    parser = subparsers.add_parser("onvif", help="Configure ONVIF")
    parser.add_argument("--ov-enable", choices=("0", "1"), required=True, help="Value for -ov_enable")
    parser.add_argument("--ov-port", required=True, help="Value for -ov_port")
    parser.add_argument("--ov-authflag", required=True, help="Value for -ov_authflag")
    parser.add_argument("--ov-forbitset", required=True, help="Value for -ov_forbitset")
    parser.add_argument("--ov-subchn", required=True, help="Value for -ov_subchn")
    parser.add_argument("--ov-snapchn", required=True, help="Value for -ov_snapchn")
    parser.add_argument("--ov-nvctype", required=True, help="Value for -ov_nvctype")


def add_p2p_parser(subparsers: argparse._SubParsersAction) -> None:
    parser = subparsers.add_parser("p2p", help="Configure P2P platform")
    parser.add_argument("--xqp2p-enable", choices=("0", "1"), required=True, help="Value for -xqp2p_enable")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Configure a his3510-based IPCAM."
    )
    parser.add_argument("--host", default=DEFAULT_HOST, help="Device host or IP")
    parser.add_argument("--port", type=int, default=DEFAULT_PORT, help="HTTP port")
    parser.add_argument("--timeout", type=float, default=10.0, help="HTTP timeout in seconds")
    parser.add_argument("--show-body", action="store_true", help="Print response body")
    parser.add_argument("--dry-run", action="store_true", help="Print request and exit")

    subparsers = parser.add_subparsers(dest="command", required=True)
    add_wifi_parser(subparsers)
    add_video_parser(subparsers)
    add_osd_parser(subparsers)
    add_humanoid_parser(subparsers)
    add_onvif_parser(subparsers)
    add_p2p_parser(subparsers)
    return parser


def config_from_args(args: argparse.Namespace) -> IPCamConfig:
    if args.command == "wifi":
        return WifiConfig(
            ssid=args.ssid,
            password=args.password,
            auth=args.auth,
            mode=args.mode,
            enc=args.enc,
            enable=args.enable,
        )

    if args.command == "video":
        return VideoConfig(
            videomode=args.videomode,
            vinorm=args.vinorm,
            profile=args.profile,
            main_stream=VideoStreamConfig(
                chn=args.main_chn,
                bps=args.main_bps,
                fps=args.main_fps,
                brmode=args.main_brmode,
                imagegrade=args.main_imagegrade,
                gop=args.main_gop,
            ),
            sub_stream=VideoStreamConfig(
                chn=args.sub_chn,
                bps=args.sub_bps,
                fps=args.sub_fps,
                brmode=args.sub_brmode,
                imagegrade=args.sub_imagegrade,
                gop=args.sub_gop,
            ),
        )

    if args.command == "osd":
        return OSDConfig(
            time_show=args.time_show,
            time_place=args.time_place,
            name_show=args.name_show,
            name=args.name,
            name_place=args.name_place,
        )

    if args.command == "humanoid":
        return HumanoidDetectionConfig(
            switch=args.switch,
            smd_enable=args.smd_enable,
            smd_rect=args.smd_rect,
            smd_gthresh=args.smd_gthresh,
        )

    if args.command == "onvif":
        return ONVIFConfig(
            ov_enable=args.ov_enable,
            ov_port=args.ov_port,
            ov_authflag=args.ov_authflag,
            ov_forbitset=args.ov_forbitset,
            ov_subchn=args.ov_subchn,
            ov_snapchn=args.ov_snapchn,
            ov_nvctype=args.ov_nvctype,
        )

    if args.command == "p2p":
        return P2PConfig(xqp2p_enable=args.xqp2p_enable)

    raise ValueError(f"Unsupported command: {args.command}")


def print_results(results: list[CommandResult], show_body: bool) -> None:
    for result in results:
        print(f"{result.command}: HTTP {result.status} {result.reason}")
        if show_body:
            print(result.body)


def main() -> int:
    args = build_parser().parse_args()
    config = config_from_args(args)

    client = IPCamClient(
        host=args.host,
        port=args.port,
        timeout=args.timeout,
    )

    try:
        validate_config(config)
        print_preview(client, config)

        if args.dry_run:
            print("dry-run mode, request not sent.")
            return 0

        results = client.apply_config(config)
        print_results(results, show_body=args.show_body)
        return 0

    except ValueError as exc:
        print(str(exc), file=sys.stderr)
        return 2
    except HTTPError as exc:
        body = exc.read().decode("utf-8", errors="replace")
        print(f"HTTP {exc.code} {exc.reason}", file=sys.stderr)

        www_auth = exc.headers.get("WWW-Authenticate")
        if www_auth:
            print(f"WWW-Authenticate: {www_auth}", file=sys.stderr)

        if body:
            print(body, file=sys.stderr)

        return 1
    except URLError as exc:
        print(f"Request failed: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
