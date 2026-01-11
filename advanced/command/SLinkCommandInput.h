#pragma once

class SLinkCommandInput {
public:
  virtual ~SLinkCommandInput() = default;

  virtual void poll() = 0;
};
