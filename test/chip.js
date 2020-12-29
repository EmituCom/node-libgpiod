const gpiod = require("..");

describe("libgpiod chip bindings", function () {
  it("should 'create' a new chip", (done) => {
    const chip0 = new gpiod.Chip("0");
    console.log(chip0);
    done();
  });

  it("should get a line from the chip", (done) => {
    const chip0 = new gpiod.Chip("gpiochip0");
    console.log(chip0);
    const line17 = new gpiod.Line(chip0, 17);
    console.log(line17);
    done();
  });

  it("should get line value", (done) => {
    const chip0 = new gpiod.Chip("gpiochip0");
    console.log(chip0);
    const line17 = new gpiod.Line(chip0, 17);
    console.log(line17);
    console.log(line17.getValue());
    done();
  });

  it("should set line value", (done) => {
    const chip0 = new gpiod.Chip("gpiochip0");
    console.log(chip0);
    const line17 = new gpiod.Line(chip0, 17);
    console.log(line17);
    console.log(line17.setValue(1));
    setTimeout(() => {
      console.log(line17.getValue());
      done();
    }, 500);
  });
});
