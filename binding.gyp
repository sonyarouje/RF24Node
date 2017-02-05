{
  "targets": [
    {
      "target_name": "nrf24Node",
      "sources": [ 
        "nrf24Node.cc",
        "RF24/RF24.cpp", 
        "RF24/utility/RPi/spi.cpp",
        "RF24/utility/RPi/bcm2835.c",
        "RF24/utility/RPi/interrupt.cpp",
        "RF24Network/RF24Network.cpp",
        "RF24Network/Sync.cpp"
        ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "RF24Network",
        "RF24"
      ],
      "link_settings": {
        "libraries": [
          "-RF24",
          "-RFNetwork"
        ]
      }   
    }
  ]
}