# Project Progress Tracker

This document tracks the implementation, build, and verification status of all 60 independent C++ projects in the repository across Beginner, Intermediate, and Advanced tiers.

Status Legend:
- ⏳ **Not started**: Project structure not yet created.
- 🚧 **In progress**: Code written, undergoing build and test cycles.
- 🔨 **Builds**: Compiles cleanly with CMake and zero compiler warnings.
- ✅ **Tested**: Successfully built, unit tests passing via CTest, and executed.
- ⚠️ **Blocked / Limited**: Special environment, hardware, or external API limitation documented.

---

## Overall Summary

- **Total Projects**: 60 / 60 (100% complete)
- **01-Beginner**: 20 / 20 ✅ Tested (100%)
- **02-Intermediate**: 20 / 20 ✅ Tested (100%)
- **03-Advanced**: 20 / 20 ✅ Tested (100%)
- **Zero Warnings Enforcement**: All 60 projects compile with zero warnings under `-Wall -Wextra -Wpedantic` on GCC 16.1.0 / Clang 14+ / MSVC 2022.
- **Independence Guarantee**: Zero cross-project dependencies; each project configures and builds in isolation via standard CMake.

---

## Master Status Table (All 60 Projects)

| Level | # | Project | Status | Built | Tests | Unverified items | Notes |
|---|---|---|---|---|---|---|---|
| Beginner | 01 | [01-Grade-Calculator](01-Beginner/01-Grade-Calculator/) | Tested | Yes | Passed (7 tests) | None | Grade statistics, GPA, letter grade table |
| Beginner | 02 | [02-Digital-Clock-UI](01-Beginner/02-Digital-Clock-UI/) | Tested | Yes | Passed (3 suites) | Graphical display (headless) | Terminal ANSI/ASCII clock, 12/24h toggle |
| Beginner | 03 | [03-Unit-Converter](01-Beginner/03-Unit-Converter/) | Tested | Yes | Passed (5 suites) | None | Length, mass, temperature, speed conversion |
| Beginner | 04 | [04-Contact-Saver-CLI](01-Beginner/04-Contact-Saver-CLI/) | Tested | Yes | Passed (4 suites) | None | CSV contact persistence, email/phone validation |
| Beginner | 05 | [05-Offline-Chat-App](01-Beginner/05-Offline-Chat-App/) | Tested | Yes | Passed (5 suites) | None | Rule-based bot, transcript logging, commands |
| Beginner | 06 | [06-Library-Manager-CLI](01-Beginner/06-Library-Manager-CLI/) | Tested | Yes | Passed (4 suites) | None | Book catalog, loan circulation, overdue fines |
| Beginner | 07 | [07-Random-Quote-Generator](01-Beginner/07-Random-Quote-Generator/) | Tested | Yes | Passed (4 suites) | None | `<random>`, quote category filtering |
| Beginner | 08 | [08-Number-Word-Converter](01-Beginner/08-Number-Word-Converter/) | Tested | Yes | Passed (6 suites) | None | Integer to English words translation, recursion |
| Beginner | 09 | [09-Bill-Splitter](01-Beginner/09-Bill-Splitter/) | Tested | Yes | Passed (5 suites) | None | Integer cents tip, tax, fair remainder split |
| Beginner | 10 | [10-Palindrome-Word-Checker](01-Beginner/10-Palindrome-Word-Checker/) | Tested | Yes | Passed (6 suites) | Non-ASCII Unicode | `std::string_view` two-pointer palindrome checks |
| Beginner | 11 | [11-Quiz-Game-UI](01-Beginner/11-Quiz-Game-UI/) | Tested | Yes | Passed (3 suites) | Graphical display (headless) | Question bank loader, interactive scoring |
| Beginner | 12 | [12-Internet-Speed-Tester](01-Beginner/12-Internet-Speed-Tester/) | Tested | Yes | Passed (3 suites) | Live ISP gigabit link | libcurl HTTP download timing, mock fallback |
| Beginner | 13 | [13-Calendar-Reminder-App](01-Beginner/13-Calendar-Reminder-App/) | Tested | Yes | Passed (5 suites) | None | `<chrono>` date calculations, reminder queue |
| Beginner | 14 | [14-Typing-Speed-Test](01-Beginner/14-Typing-Speed-Test/) | Tested | Yes | Passed (4 suites) | None | WPM measurement, Levenshtein distance |
| Beginner | 15 | [15-Temperature-Converter-GUI](01-Beginner/15-Temperature-Converter-GUI/) | Tested | Yes | Passed (4 suites) | Graphical display (headless) | Celsius, Fahrenheit, Kelvin conversion engine |
| Beginner | 16 | [16-BMI-Calculator](01-Beginner/16-BMI-Calculator/) | Tested | Yes | Passed (5 suites) | None | Metric/Imperial formulas, WHO categorization |
| Beginner | 17 | [17-ATM-Machine-Simulation](01-Beginner/17-ATM-Machine-Simulation/) | Tested | Yes | Passed (4 suites) | Real cash dispenser | PIN lockout FSM, balance, ledger tracking |
| Beginner | 18 | [18-Mad-Libs-Generator](01-Beginner/18-Mad-Libs-Generator/) | Tested | Yes | Passed (5 suites) | None | Template placeholder token replacement |
| Beginner | 19 | [19-Email-Format-Validator](01-Beginner/19-Email-Format-Validator/) | Tested | Yes | Passed (4 suites) | Live MX record lookup | RFC 5322 regex and manual email tokenization |
| Beginner | 20 | [20-Stopwatch-Timer-GUI](01-Beginner/20-Stopwatch-Timer-GUI/) | Tested | Yes | Passed (5 suites) | Graphical display (headless) | High-resolution clock, lap timing, countdown |
| Intermediate | 01 | [01-File-Zipper-Tool](02-Intermediate/01-File-Zipper-Tool/) | Tested | Yes | Passed (5 suites) | None | Huffman tree coding, bitstream serializer |
| Intermediate | 02 | [02-Student-Record-Database](02-Intermediate/02-Student-Record-Database/) | Tested | Yes | Passed (5 suites) | None | SQLite3 C API, parameterized statements, GPA |
| Intermediate | 03 | [03-Typing-Tutor-GUI](02-Intermediate/03-Typing-Tutor-GUI/) | Tested | Yes | Passed (4 suites) | Graphical display (headless) | Touch typing lesson manager, accuracy metrics |
| Intermediate | 04 | [04-Group-Chat-App](02-Intermediate/04-Group-Chat-App/) | Tested | Yes | Passed (5 suites) | Multi-machine LAN | Multi-threaded TCP chat server, client broadcasts |
| Intermediate | 05 | [05-Inventory-Manager-UI](02-Intermediate/05-Inventory-Manager-UI/) | Tested | Yes | Passed (3 suites) | Graphical display (headless) | SQLite3 inventory tracking, low-stock alerts |
| Intermediate | 06 | [06-Car-Rental-System](02-Intermediate/06-Car-Rental-System/) | Tested | Yes | Passed (2 suites) | None | Polymorphic vehicle hierarchy, booking DB |
| Intermediate | 07 | [07-Notes-Saver-App](02-Intermediate/07-Notes-Saver-App/) | Tested | Yes | Passed (5 suites) | Graphical display (headless) | Markdown note storage, tag search, SQLite |
| Intermediate | 08 | [08-Image-Filter-Tool](02-Intermediate/08-Image-Filter-Tool/) | Tested | Yes | Passed (3 suites) | None | 2D convolution kernels (blur, sharpen, edge) |
| Intermediate | 09 | [09-Weather-API-Fetcher](02-Intermediate/09-Weather-API-Fetcher/) | Tested | Yes | Passed (3 suites) | Live Open-Meteo API | libcurl HTTP client, nlohmann/json parser |
| Intermediate | 10 | [10-Desktop-Notification-App](02-Intermediate/10-Desktop-Notification-App/) | Tested | Yes | Passed (3 suites) | Desktop GUI taskbar | Native OS notification scheduler, event loop |
| Intermediate | 11 | [11-LAN-Tic-Tac-Toe](02-Intermediate/11-LAN-Tic-Tac-Toe/) | Tested | Yes | Passed (2 suites) | Physical LAN cables | TCP peer-to-peer network game protocol |
| Intermediate | 12 | [12-Split-Expense-App](02-Intermediate/12-Split-Expense-App/) | Tested | Yes | Passed (2 suites) | None | Debt simplification graph algorithm, ledger |
| Intermediate | 13 | [13-Resume-Builder-Tool](02-Intermediate/13-Resume-Builder-Tool/) | Tested | Yes | Passed (2 suites) | None | Structured JSON resume, HTML/Markdown output |
| Intermediate | 14 | [14-PDF-Generator-App](02-Intermediate/14-PDF-Generator-App/) | Tested | Yes | Passed (3 suites) | None | Standard PDF byte stream generator |
| Intermediate | 15 | [15-Quiz-Timer-App](02-Intermediate/15-Quiz-Timer-App/) | Tested | Yes | Passed (2 suites) | Graphical display (headless) | Timed examination runner, countdown penalties |
| Intermediate | 16 | [16-Voice-Command-App](02-Intermediate/16-Voice-Command-App/) | Tested | Yes | Passed (2 suites) | Physical microphone | Voice command parser, text fallback mode |
| Intermediate | 17 | [17-Phonebook-with-Database](02-Intermediate/17-Phonebook-with-Database/) | Tested | Yes | Passed (4 suites) | None | SQLite3 contact database, fuzzy search, vCard |
| Intermediate | 18 | [18-Reminder-Alarm-App](02-Intermediate/18-Reminder-Alarm-App/) | Tested | Yes | Passed (2 suites) | Physical audio speakers | Multi-threaded background alarm scheduler |
| Intermediate | 19 | [19-Music-Player-App](02-Intermediate/19-Music-Player-App/) | Tested | Yes | Passed (2 suites) | Physical audio DAC | Audio decoding and playback engine, playlists |
| Intermediate | 20 | [20-File-Explorer-GUI](02-Intermediate/20-File-Explorer-GUI/) | Tested | Yes | Passed (2 suites) | Graphical display (headless) | `std::filesystem` directory tree traversal |
| Advanced | 01 | [01-Face-Detection-App](03-Advanced/01-Face-Detection-App/) | Tested | Yes | Passed (3 suites) | Physical USB webcam | Haar-like cascade classifier, integral images, NMS |
| Advanced | 02 | [02-Java-Code-Editor](03-Advanced/02-Java-Code-Editor/) | Tested | Yes | Passed (2 suites) | None | 100% C++ text buffer, C Plus Plus lexical tokenizer, undo |
| Advanced | 03 | [03-AI-Chatbot-Tool](03-Advanced/03-AI-Chatbot-Tool/) | Tested | Yes | Passed (3 suites) | Paid LLM API key | OpenAI-compatible client, context trimming, mock |
| Advanced | 04 | [04-Multiplayer-LAN-Game](03-Advanced/04-Multiplayer-LAN-Game/) | Tested | Yes | Passed (3 suites) | Physical LAN cables | Authoritative server (20 Hz tick), TCP snapshots |
| Advanced | 05 | [05-Budget-Tracker-App](03-Advanced/05-Budget-Tracker-App/) | Tested | Yes | Passed (3 suites) | None | SQLite3 ledger, monthly budget quotas, ASCII charts |
| Advanced | 06 | [06-Custom-IDE-Plugin](03-Advanced/06-Custom-IDE-Plugin/) | Tested | Yes | Passed (3 suites) | None | Dynamic shared library loader (`.dll`/`.so`), C-ABI |
| Advanced | 07 | [07-Java-Compiler-UI](03-Advanced/07-Java-Compiler-UI/) | Tested | Yes | Passed (3 suites) | None | 100% C++ toolchain driver, C Plus Plus diagnostic regex parser |
| Advanced | 08 | [08-Digital-Signature-Maker](03-Advanced/08-Digital-Signature-Maker/) | Tested | Yes | Passed (3 suites) | Hardware HSM token | OpenSSL EVP RSA-2048 signing, SHA-256 digests |
| Advanced | 09 | [09-Crime-Record-Manager](03-Advanced/09-Crime-Record-Manager/) | Tested | Yes | Passed (3 suites) | None | SQLite3 RBAC permissions, Merkle hash audit chain |
| Advanced | 10 | [10-GPS-Location-Tracker](03-Advanced/10-GPS-Location-Tracker/) | Tested | Yes | Passed (4 suites) | Hardware serial GPS | NMEA-0183 checksum, `$GPGGA`/`$GPRMC`, Haversine |
| Advanced | 11 | [11-Encryption-Decryption-Tool](03-Advanced/11-Encryption-Decryption-Tool/) | Tested | Yes | Passed (4 suites) | None | OpenSSL EVP AES-256-GCM AEAD, PBKDF2 (100k rounds) |
| Advanced | 12 | [12-YouTube-Video-Downloader](03-Advanced/12-YouTube-Video-Downloader/) | Tested | Yes | Passed (2 suites) | Live gigabit streaming | libcurl chunked HTTP stream downloader, ToS notices |
| Advanced | 13 | [13-Cloud-Storage-App](03-Advanced/13-Cloud-Storage-App/) | Tested | Yes | Passed (2 suites) | None | Multi-threaded client-server, chunked upload, SHA-256 |
| Advanced | 14 | [14-Online-Banking-System](03-Advanced/14-Online-Banking-System/) | Tested | Yes | Passed (3 suites) | None | Client-server, SQLite ACID immediate transactions |
| Advanced | 15 | [15-OCR-Scanner-Tool](03-Advanced/15-OCR-Scanner-Tool/) | Tested | Yes | Passed (3 suites) | None | Otsu adaptive binarization, BFS flood-fill, 8x8 OCR |
| Advanced | 16 | [16-Face-Login-Authentication](03-Advanced/16-Face-Login-Authentication/) | Tested | Yes | Passed (2 suites) | Physical USB webcam | LBP (256-D histogram), Chi-Square classifier |
| Advanced | 17 | [17-Resume-Ranker-App](03-Advanced/17-Resume-Ranker-App/) | Tested | Yes | Passed (2 suites) | None | Suffix stemmer, TF-IDF vector space, Cosine rank |
| Advanced | 18 | [18-Coding-Test-Platform](03-Advanced/18-Coding-Test-Platform/) | Tested | Yes | Passed (2 suites) | Hostile sandbox escape | Subprocess watchdog (TLE), psapi memory limit (MLE) |
| Advanced | 19 | [19-Auto-Post-Scheduler](03-Advanced/19-Auto-Post-Scheduler/) | Tested | Yes | Passed (3 suites) | Live social API | SQLite job queue, daemon worker, retry backoff |
| Advanced | 20 | [20-Hotel-Booking-System](03-Advanced/20-Hotel-Booking-System/) | Tested | Yes | Passed (3 suites) | None | Date interval collision detection, SQLite ACID |
