
target_sources(${PROJECT_NAME}
    PRIVATE
    # {{BEGIN_TARGET_SOURCES}}
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_context_restore.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_context_save.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_interrupt_control.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_interrupt_disable.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_interrupt_restore.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_schedule.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_secure_stack_allocate.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_secure_stack_free.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_secure_stack_initialize.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_stack_build.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_system_return.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_timer_interrupt.S

	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_secure_stack.c
	${CMAKE_CURRENT_LIST_DIR}/src/txe_thread_secure_stack_allocate.c
	${CMAKE_CURRENT_LIST_DIR}/src/txe_thread_secure_stack_free.c
    # {{END_TARGET_SOURCES}}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc
)
