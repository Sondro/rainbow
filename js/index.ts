/// <reference path="./index.d.ts" />

/* eslint-disable no-undef, no-unused-vars */

interface Demo {
  deinit(): void;
  update(dt: number): void;
}

class Audial implements Demo {
  private label: Rainbow.Label;
  private music?: Rainbow.Audio.Sound;
  private sound?: Rainbow.Audio.Sound;
  private thread: Duktape.Thread;
  private time: number = 0;
  private timeout: number;

  constructor(width: number, height: number) {
    console.log("Demo: Audial");

    this.label = new Rainbow.Label()
      .alignment(Rainbow.TextAlignment.Center)
      .font("OpenSans-Light.ttf")
      .fontSize(21)
      .position({ x: width * 0.5, y: height * 0.5 });
    Rainbow.RenderQueue.add(this.label);

    const Thread = Duktape.Thread;
    this.thread = new Thread(x => {
      const Audio = Rainbow.Audio;
      const soundPath = "sfx.ogg";
      const streamPath = "bgm.ogg";

      this.label.text(`Loading '${streamPath}' for streaming...`);
      this.music = Audio.loadStream(streamPath);
      if (!this.music) {
        this.label.text(`Failed to load '${streamPath}'`);
        return;
      }

      Thread.yield(1000);

      this.label.text(`Streaming '${streamPath}'...`);
      let channel = Audio.play(this.music);
      if (!channel) {
        this.label.text(`Failed to stream '${streamPath}'`);
        return;
      }

      Thread.yield(1000);

      this.label.text("Paused");
      Audio.pause(channel);
      Thread.yield(1000);

      this.label.text("Resume streaming...");
      Audio.play(channel);
      Thread.yield(1000);

      this.label.text("Stop streaming");
      Audio.stop(channel);
      Thread.yield(1000);

      this.label.text(`Loading '${soundPath}' into memory...`);
      this.sound = Audio.loadSound(soundPath);
      if (!this.sound) {
        this.label.text(`Failed to load '${soundPath}'`);
        return;
      }

      Thread.yield(1000);

      this.label.text(`Playing '${soundPath}'...`);
      channel = Audio.play(this.sound);
      if (!channel) {
        this.label.text(`Failed to play '${soundPath}'`);
        return;
      }

      Thread.yield(1000);

      this.label.text("Paused");
      Audio.pause(channel);
      Thread.yield(1000);

      this.label.text("Resume playing...");
      Audio.play(channel);
      Thread.yield(1000);

      this.label.text("Streaming (different channel)...");
      channel = Audio.play(this.music);
      if (!channel) {
        this.label.text(`Failed to play '${streamPath}'`);
        return;
      }

      Thread.yield(1000);

      this.label.text("Delete both buffer and stream");
      this.music = Audio.release(this.music);
      this.sound = Audio.release(this.sound);
      Thread.yield(1500);

      this.label.text("Load into buffer and play (overflow)");
      this.sound = Audio.loadSound(soundPath);
      if (!this.sound) {
        this.label.text(`Failed to load '${soundPath}'`);
        return;
      }

      for (let i = 0; i < 40; ++i) {
        Audio.play(this.sound);
        Thread.yield(100);
      }
      Thread.yield(1000);

      this.label.text("Clear");
      this.sound = Audio.release(this.sound);
      Thread.yield(1500);

      this.label.text("Loop once");
      this.music = Audio.loadSound(soundPath);
      if (!this.music) {
        this.label.text(`Failed to load '${streamPath}'`);
        return;
      }

      channel = Audio.play(this.music);
      if (!channel) {
        this.label.text(`Failed to play '${soundPath}'`);
        return;
      }

      Audio.setLoopCount(channel, 1);
      Thread.yield(1000 * 60 * 10);
    });

    this.timeout = Thread.resume(this.thread);
  }

  public deinit(): void {
    this.music && Rainbow.Audio.release(this.music);
    this.sound && Rainbow.Audio.release(this.sound);
    Rainbow.RenderQueue.erase(this.label);
  }

  public update(dt: number): void {
    this.time += dt;
    if (this.time >= this.timeout) {
      this.time = 0;
      this.timeout = Duktape.Thread.resume(this.thread);
    }
  }
}

class Labels implements Demo {
  private label: Rainbow.Label;
  private screen: { width: number; height: number };
  private text = [
    "Open Sans\nAaBbCcDdEeFfGgHhIi\nJjKkLlMmNnOoPpQqRr\nSsTtUuVvWwXxYyZz",
    "Grumpy wizards make\ntoxic brew for the\nevil Queen and Jack.",
    "The quick brown fox jumps\nover the lazy dog."
  ];
  private yOffset = [0.55, 0.63, 0.71];
  private thread: Duktape.Thread;
  private time: number = 0;
  private timeout: number;

  constructor(width: number, height: number) {
    console.log("Demo: Labels");

    this.screen = { width, height };

    this.label = new Rainbow.Label()
      .alignment(Rainbow.TextAlignment.Center)
      .font("OpenSans-Light.ttf")
      .fontSize(60);
    Rainbow.RenderQueue.add(this.label);

    const Thread = Duktape.Thread;

    let frame = 0;
    this.thread = new Thread(x => {
      const { floor, random } = Math;
      // eslint-disable-next-line no-constant-condition
      while (true) {
        const stanza = this.text[frame];
        const lines = (stanza.match(/\n/g) || []).length;
        this.label
          .position({
            x: this.screen.width * 0.5,
            y: this.screen.height * this.yOffset[lines - 1]
          })
          .text(stanza);
        Thread.yield(3000);

        frame = (frame + 1) % this.text.length;

        // Test that colour is set for future strings
        this.label.color({
          r: floor(random() * 256),
          g: floor(random() * 256),
          b: floor(random() * 256),
          a: 255
        });

        Thread.yield(0);
      }
    });

    this.timeout = Thread.resume(this.thread);
  }

  public deinit(): void {
    Rainbow.RenderQueue.erase(this.label);
  }

  public update(dt: number): void {
    this.time += dt;
    if (this.time >= this.timeout) {
      this.time = 0;
      this.timeout = Duktape.Thread.resume(this.thread);
    }
  }
}

class Shaker implements Demo {
  private static MAX_NUM_SPRITES = 256;

  private screen: { width: number; height: number };
  private texture: Rainbow.Texture;
  private batches: Rainbow.SpriteBatch[] = [];
  private sprites: Rainbow.Sprite[] = [];
  private frameTimes: number[] = [];

  constructor(width: number, height: number) {
    console.log("Demo: Shaker");

    this.screen = { width, height };
    this.texture = new Rainbow.Texture("p1_spritesheet.png");
  }

  public deinit(): void {
    for (let i = this.batches.length - 1; i >= 0; --i) {
      Rainbow.RenderQueue.erase(this.batches[i]);
    }
  }

  public update(dt: number): void {
    if (this.frameTimes.length >= 10) {
      this.frameTimes.shift();
    }
    this.frameTimes.push(dt);
    const average =
      this.frameTimes.reduce((total, value) => total + value, 0) /
      this.frameTimes.length;

    if (average < 20) {
      const batch = new Rainbow.SpriteBatch(Shaker.MAX_NUM_SPRITES);
      batch.setTexture(this.texture);

      for (let i = 0; i < Shaker.MAX_NUM_SPRITES; ++i) {
        const sprite = batch
          .createSprite(72, 97)
          .position({
            x: Math.random() * this.screen.width,
            y: Math.random() * this.screen.height
          })
          .texture({ left: 0, bottom: 0, width: 72, height: 97 });
        this.sprites.push(sprite);
      }

      Rainbow.RenderQueue.add(batch);
      this.batches.push(batch);
    }

    const { PI, random } = Math;
    const numSprites = this.sprites.length;
    for (let i = 0; i < numSprites; ++i) {
      this.sprites[i].rotate(random() * PI);
    }
  }
}

class Stalker implements Demo {
  private texture: Rainbow.Texture;
  private batch: Rainbow.SpriteBatch;
  private sprite: Rainbow.Sprite;
  private animation: Rainbow.Animation;
  private pointersDown: typeof Rainbow.Input.pointersDown;
  private pointersMoved: typeof Rainbow.Input.pointersMoved;

  constructor(width: number, height: number) {
    console.log("Demo: Stalker");

    this.texture = new Rainbow.Texture("p1_spritesheet.png");

    this.batch = new Rainbow.SpriteBatch(1);
    this.batch.setTexture(this.texture);

    const walkingFrames = [
      { left: 0, bottom: 0, width: 72, height: 97 },
      { left: 73, bottom: 0, width: 72, height: 97 },
      { left: 146, bottom: 0, width: 72, height: 97 },
      { left: 0, bottom: 98, width: 72, height: 97 },
      { left: 73, bottom: 98, width: 72, height: 97 },
      { left: 146, bottom: 98, width: 72, height: 97 },
      { left: 219, bottom: 0, width: 72, height: 97 },
      { left: 292, bottom: 0, width: 72, height: 97 },
      { left: 219, bottom: 98, width: 72, height: 97 },
      { left: 365, bottom: 0, width: 72, height: 97 },
      { left: 292, bottom: 98, width: 72, height: 97 }
    ];
    const { pointersDown, pointersMoved } = Rainbow.Input;

    this.sprite = this.batch
      .createSprite(72, 97)
      .texture(walkingFrames[0])
      .position(
        pointersDown.length > 0
          ? pointersDown[0]
          : { x: width * 0.5, y: height * 0.5 }
      );

    this.animation = new Rainbow.Animation(this.sprite, walkingFrames, 24, 0);
    this.animation.start();

    this.pointersDown = pointersDown;
    this.pointersMoved = pointersMoved;

    Rainbow.RenderQueue.add(this.batch);
    Rainbow.RenderQueue.add(this.animation);
  }

  public deinit(): void {
    Rainbow.RenderQueue.erase(this.animation);
    Rainbow.RenderQueue.erase(this.batch);
  }

  public update(dt: number): void {
    if (this.pointersMoved.length > 0) {
      this.sprite.position(this.pointersMoved[0]);
    } else if (this.pointersDown.length > 0) {
      this.sprite.position(this.pointersDown[0]);
    }
  }
}

let State: {
  createDemo: (() => Demo)[];
  currentDemo: number;
  demo: Demo;
  label: Rainbow.Label;
  labelPos: { x: number; y: number };
};

function init(width: number, height: number) {
  const createDemo = [
    () => new Audial(width, height),
    () => new Labels(width, height),
    () => new Shaker(width, height),
    () => new Stalker(width, height)
  ];
  const currentDemo = 3;
  const demo = createDemo[currentDemo]();

  const margin = 16;
  const labelPos = { x: width - margin, y: margin };
  const label = new Rainbow.Label()
    .alignment(Rainbow.TextAlignment.Right)
    .font("OpenSans-Light.ttf")
    .fontSize(24)
    .position(labelPos)
    .text("NEXT DEMO");
  Rainbow.RenderQueue.add(label);

  State = { createDemo, currentDemo, demo, label, labelPos };
}

function update(dt: number) {
  const pointersDown = Rainbow.Input.pointersDown;
  if (pointersDown.length > 0) {
    const { label, labelPos } = State;
    const padding = 8;
    const p = pointersDown[0];
    const didHit =
      p.x >= labelPos.x - label.width() - padding &&
      p.x <= labelPos.x + padding &&
      p.y >= labelPos.y - padding &&
      p.y <= labelPos.y + label.height() + padding;
    if (didHit) {
      const { createDemo, currentDemo, demo } = State;

      demo.deinit();
      Duktape.gc();

      const nextDemo = (currentDemo + 1) % createDemo.length;
      const newDemo = createDemo[nextDemo]();
      Duktape.gc();

      State = {
        ...State,
        currentDemo: nextDemo,
        demo: newDemo
      };

      return;
    }
  }

  State.demo.update(dt);
}
