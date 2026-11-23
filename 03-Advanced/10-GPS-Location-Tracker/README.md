# 10-GPS-Location-Tracker

## 1. Project Overview
10-GPS-Location-Tracker is an aviation and marine-grade GPS telemetry processor and location tracker developed in standard C++17. The system decodes raw NMEA-0183 serial data streams (`$GPGGA`, `$GPRMC`), performs 8-bit XOR checksum validation, translates spherical coordinates into decimal degrees, and calculates geodesic distances and navigation headings using the Haversine formula and forward azimuth equations.

## 2. Problem Statement
Raw GPS receivers transmit spatial telemetry via the legacy ASCII NMEA-0183 protocol. Embedded navigational instruments require deterministic, low-latency parsing to filter corrupted packets, convert geographic degrees/minutes into floating-point coordinates, and compute accumulated path lengths and ground velocities.

## 3. Objectives
- Implement an NMEA-0183 framing and XOR checksum validator.
- Parse `$GPGGA` (fix quality, satellite constellation size, altitude) and `$GPRMC` (speed in knots, heading, active fix status).
- Convert NMEA `DDMM.MMMM` coordinate formats to decimal degrees with cardinal direction adjustment (S/W negative).
- Implement Haversine great-circle distance and initial bearing calculations on the WGS-84 reference sphere.
- Provide a synthetic flight path telemetry generator (`GPSSimulator`) for automated test verification without requiring physical GPS hardware.

## 4. Features
- **NMEA Checksum Verification**: Discards frames failing 8-bit XOR validation.
- **Geodesic Navigation Engine**: Computes spherical distance in kilometers and nautical knots conversion to km/h.
- **Waypoint Trajectory Accumulation**: Calculates leg-by-leg distances and cumulative journey mileage.
- **Hardware-Free Simulation**: Generates synthetically verified flight legs (e.g., London to Paris) for headless CI testing.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::string`, `std::optional`, `std::cmath`, `std::iomanip`).
- **External Services**: None. Fully self-contained.
- **Database**: None required.
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
10-GPS-Location-Tracker/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── GeoMath.hpp
│   ├── GPSSimulator.hpp
│   └── NMEAParser.hpp
├── src/
│   ├── GPSSimulator.cpp
│   ├── NMEAParser.cpp
│   └── main.cpp
└── tests/
    └── test_gps_tracker.cpp
```

## 7. Installation / Prerequisites
- CMake 3.20 or newer.
- GCC 11+, Clang 14+, or MSVC 2022.
- doctest headers.

## 8. Build and Run Instructions
### Windows & Linux Build
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Running the Application
```bash
./build/GPSLocationTracker
```

### Automated Verification Mode
```bash
./build/GPSLocationTracker --test-run
```

## 9. Testing Instructions
Run CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
========================================
  GPS Location Tracker & NMEA Decoder   
========================================
Processing simulated NMEA telemetry stream (6 sentences)...

[NMEA FIX 120000.00]
  Coordinates: 51.50440, -0.12780
  Altitude:    25.40000 m
  Satellites:  8
  Speed:       120.50000 kts (223.16600 km/h)
  Heading:     145.20000 deg

[NMEA FIX 121500.00]
  Coordinates: 50.50000, 1.00000
  Altitude:    150.00000 m
  Satellites:  9
  Leg Delta:   136.78200 km (Bearing: 147.21000 deg)
----------------------------------------
Total Route Distance: 343.42150 km
[Test Mode] GPS NMEA tracking and geodesic verification passed.
```

## 11. Known Limitations
- Flat Earth projections are avoided; distance uses the spherical earth approximation (radius 6371.0 km) rather than full ellipsoidal Vincenty geodesic formulas, which provides < 0.5% error margin suitable for navigation tracking.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows with 0 warnings (`-Wall -Wextra -Wpedantic`).
- **Run**: Verified NMEA parsing, Haversine computations, and telemetry output with `--test-run`.
- **Tested**: All 4 test cases passed under CTest.
