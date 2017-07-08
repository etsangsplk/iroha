add_library(networking
    impl/logger_service.cpp
    peer_communication_stub.cpp
    )

target_link_libraries(networking PUBLIC
    rxcpp
    model
    ametsuchi
    )
