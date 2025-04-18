#!/nethome/zfan87/pynqtrial/venv/bin/python3
import argparse 
from pathlib import Path
from pynq.pl_server import hwh_parser, embedded_device

def main():
    parser = argparse.ArgumentParser(description='Create .xclbin file from .hwh file')
    parser.add_argument('hwh_file', type=Path, help='the .hwh file')
    args = parser.parse_args()
    
    hwh_data = args.hwh_file.read_text()
    hwh = hwh_parser._HWHUltrascale(hwh_data=hwh_data)
    xclbin_data = embedded_device._create_xclbin(hwh.mem_dict)
    args.hwh_file.with_suffix('.xclbin').write_bytes(xclbin_data)
    
if __name__ == '__main__':
    main()
