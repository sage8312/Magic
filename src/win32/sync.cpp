﻿// ---------------------------------------------------------------------------
//
//  Author
//      Park DongHa     | luncliff@gmail.com
//
//  License
//      CC BY 4.0
//
// ---------------------------------------------------------------------------
#include <magic/sync.h>

namespace magic {

section::section(uint16_t spin) noexcept {
  ::InitializeCriticalSectionAndSpinCount(this, spin);
}

section::~section() noexcept { ::DeleteCriticalSection(this); }

bool section::try_lock() noexcept {
  return ::TryEnterCriticalSection(this) == TRUE;
}

void section::lock() noexcept { return ::EnterCriticalSection(this); }

void section::unlock() noexcept { return ::LeaveCriticalSection(this); }

wait_group::wait_group() noexcept(false) {
  this->eve = CreateEvent(nullptr,  // default security attributes
                          false, // auto-reset event object
                          false, // initial state is nonsignaled
                          nullptr); // unnamed object

  if (this->eve == INVALID_HANDLE_VALUE) {
    const auto eval = static_cast<int>(GetLastError());
    const auto &&emsg = std::system_category().message(eval);

    throw std::runtime_error{emsg};
  }
}

wait_group::~wait_group() noexcept {
  if (this->eve != INVALID_HANDLE_VALUE)
    CloseHandle(this->eve);
}

void wait_group::add(uint32_t count) noexcept { this->ref.fetch_add(count); }

void wait_group::done() noexcept {
  this->ref.fetch_sub(1);
  if (this->ref == 0)
    SetEvent(this->eve);
}

void wait_group::wait(uint32_t timeout) noexcept {
  static_assert(sizeof(uint32_t) == sizeof(DWORD), "Invalid timeout type");

  DWORD reason = NO_ERROR;
  while (this->ref > 0 && (this->eve != INVALID_HANDLE_VALUE)) {
    // This makes APC available.
    // expecially for Overlapped I/O
    reason = WaitForSingleObjectEx(eve, timeout, TRUE);
    if (reason != WAIT_IO_COMPLETION) {
      CloseHandle(this->eve);
      this->eve = INVALID_HANDLE_VALUE;
    }
  }
  return;
}

} // namespace magic
