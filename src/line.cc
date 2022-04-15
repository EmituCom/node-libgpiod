#include "line.hh"

Nan::Persistent<v8::Function> Line::constructor;

NAN_MODULE_INIT(Line::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Line").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "getValue", getValue);
  Nan::SetPrototypeMethod(tpl, "setValue", setValue);
  Nan::SetPrototypeMethod(tpl, "requestInputMode", requestInputMode);
  Nan::SetPrototypeMethod(tpl, "requestOutputMode", requestOutputMode);
  Nan::SetPrototypeMethod(tpl, "release", release);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("Line").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

Line::Line(Chip *chip, unsigned int pin) {
  line = gpiod_chip_get_line(chip->getNativeChip(), pin);
  if (!line) Nan::ThrowError("Unable to open GPIO line ");
}

Line::~Line() {
  if ( !line) return;
  gpiod_line_close_chip(line);
  line = NULL;
}

NAN_METHOD(Line::New) {
  if (info.IsConstructCall()) {
    Chip *chip = Nan::ObjectWrap::Unwrap<Chip>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    unsigned int pin = Nan::To<unsigned int>(info[1]).FromJust();
    Line *obj = new Line(chip, pin);
    if ( !obj->line) return;
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {info[0]};
    v8::Local<v8::Function> cons = Nan::New(constructor);
    info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
  }
}

NAN_METHOD(Line::getValue) {
  Line *obj = Nan::ObjectWrap::Unwrap<Line>(info.This());
  if ( !obj->line) {
    Nan::ThrowError( "::getValue() for line==NULL");
    return;  }
  int ret = gpiod_line_get_value(obj->getNativeLine());
  if(-1 == ret){
    Nan::ThrowError( "::getValue() failed");
  } else info.GetReturnValue().Set(ret);
}

NAN_METHOD(Line::setValue) {
  Line *obj = Nan::ObjectWrap::Unwrap<Line>(info.This());
  if ( !obj->line) {
    Nan::ThrowError( "::setValue() for line==NULL");
    return;  }
  unsigned int value = Nan::To<unsigned int>(info[0]).FromJust();
  if(-1 == gpiod_line_set_value(obj->getNativeLine(), value))
    Nan::ThrowError( "::setValue() failed");
}

NAN_METHOD(Line::requestInputMode) {
  Line *obj = Nan::ObjectWrap::Unwrap<Line>(info.This());
  if ( !obj->line) {
    Nan::ThrowError( "::requestInputMode() for line==NULL");
    return;  }
  Nan::Utf8String consumer(info[0]);
  if (-1 == gpiod_line_request_input(obj->getNativeLine(), *consumer))
    Nan::ThrowError( "::requestInputMode() failed");
}

NAN_METHOD(Line::requestOutputMode) {
  Line *obj = Nan::ObjectWrap::Unwrap<Line>(info.This());
  if ( !obj->line) {
      Nan::ThrowError( "::requestOutputMode() for line==NULL");
      return;  }
  unsigned int value = 0;
  v8::Local<v8::Value> defaultValue = info[0];
  if (!defaultValue->IsUndefined() && defaultValue->IsNumber()) {
    unsigned int val = Nan::To<unsigned int>(defaultValue).FromJust();
    if (val > 1) {
      Nan::ThrowError( "::requestOutputMode() value is not in {0,1} range");
      return;
    }
    value = val;
  }
  Nan::Utf8String consumer(info[1]);
  if (-1 == gpiod_line_request_output(obj->getNativeLine(), *consumer, value))
    Nan::ThrowError( "::requestOutputMode() failed");
}


NAN_METHOD(Line::release) {
  Line *obj = Nan::ObjectWrap::Unwrap<Line>(info.This());
  if ( !obj->getNativeLine()) return;
  gpiod_line_release(obj->getNativeLine());
  obj->line = NULL;
}

gpiod_line *Line::getNativeLine() {
  return line;
}
