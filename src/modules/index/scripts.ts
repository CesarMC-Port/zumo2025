import { animate, random, stagger } from 'animejs';
const top = './assets/top.png';          // robot
const boxTexture = './assets/box.png';   // si quieres textura para las cajas

export async function initSimulator() {
  const canvas = document.getElementById("arena") as HTMLCanvasElement;
  const ctx = canvas.getContext("2d")!;

  // -------------------------------------
  // LOAD IMAGES
  // -------------------------------------
  const robotImg = await loadImage(top);
  const boxImg = await loadImage(boxTexture);

  function loadImage(src: string): Promise<HTMLImageElement> {
    return new Promise((resolve) => {
      const img = new Image();
      img.src = src;
      img.onload = () => resolve(img);
    });
  }

  // ARENA
  const arenaDiameter = 680;
  const arenaRadius = arenaDiameter / 2;
  const arenaX = canvas.width / 2;
  const arenaY = canvas.height / 2;

  // ROBOT
  const robotSize = 100;
  let robot = {
    x: arenaX,
    y: arenaY + 80,
    angle: 0,
    speed: 0.8,
    detectingLine: true,
    state: "SEARCH" as "SEARCH" | "LINE" | "PUSH" | "ALIGN",
    lastSeen: null as number | null
  };

  // BOXES
  const boxSize = 100;
  let boxes: { x: number; y: number }[] = [];

  // -------------------------------------
  // RESET
  // -------------------------------------
  function resetSimulation() {
    robot.x = arenaX;
    robot.y = arenaY + 80;
    robot.angle = 0;
    robot.state = "SEARCH";

    boxes = []; // ← NO autogenerar cajas
  }

  // -------------------------------------
  // CREAR UNA ÚNICA CAJA CON CLICK
  // -------------------------------------
  canvas.addEventListener("click", (e) => {
    if (boxes.length >= 1) return; // Solo 1 caja permitida

    const rect = canvas.getBoundingClientRect();
    const x = e.clientX - rect.left;
    const y = e.clientY - rect.top;

    const dist = Math.hypot(x - arenaX, y - arenaY);
    if (dist < arenaRadius - 60) {
      boxes.push({ x, y });
    }
  });

  // -------------------------------------
  // SENSOR SIMULATION
  // -------------------------------------
  function detectLine() {
    const dist = Math.hypot(robot.x - arenaX, robot.y - arenaY);
    return dist > arenaRadius - 30;
  }

  function detectFrontBox() {
    return boxes.find((b) => {
      let dx = b.x - robot.x;
      let dy = b.y - robot.y;
      let dist = Math.hypot(dx, dy);
      let angleToBox = Math.atan2(dy, dx);
      let diff = Math.abs(normalize(angleToBox - robot.angle));
      return dist < 100 && diff < 20;
    });
  }

  function detectSideBoxes() {
    return boxes.map((b) => {
      let dx = b.x - robot.x;
      let dy = b.y - robot.y;
      let dist = Math.hypot(dx, dy);
      let angleToBox = Math.atan2(dy, dx);
      let diff = normalize(angleToBox - robot.angle);
      return { box: b, dist, angle: diff };
    });
  }

  function normalize(a: number) {
    return ((a + Math.PI) % (2 * Math.PI)) - Math.PI;
  }

  // -------------------------------------
  // ROBOT BEHAVIOR
  // -------------------------------------
  function updateRobot() {
    if (detectLine()) {
      robot.state = "LINE";
    }

    if (robot.state === "LINE") {
      move(-robot.speed * 2);
      robot.angle += 0.15;
      if (!detectLine()) robot.state = "SEARCH";
      return;
    }

    let frontBox = detectFrontBox();
    if (frontBox) {
      robot.state = "PUSH";
      pushTowards(frontBox);
      return;
    }

    let sideInfo = detectSideBoxes();
    let closest = sideInfo.sort((a, b) => a.dist - b.dist)[0];

    if (closest && closest.dist < 850) {
      robot.state = "ALIGN";
      if (closest.angle > 0.2) robot.angle += 0.05;
      else if (closest.angle < -0.2) robot.angle -= 0.05;
      else move(robot.speed);
      return;
    }

    robot.state = "SEARCH";
    robot.angle += 0.03;
    robot.angle = normalize(robot.angle);
  }

  function move(speed: number) {
    robot.x += Math.cos(robot.angle) * speed;
    robot.y += Math.sin(robot.angle) * speed;
  }

  function pushTowards(box: { x: number; y: number }) {
    let dx = box.x - robot.x;
    let dy = box.y - robot.y;
    robot.angle = Math.atan2(dy, dx);
    move(robot.speed * 2);

    box.x += dx * 0.07;
    box.y += dy * 0.07;

    let dist = Math.hypot(box.x - arenaX, box.y - arenaY);
    if (dist > arenaRadius) {
      boxes = boxes.filter((b) => b !== box);
    }
  }

  // -------------------------------------
  // DRAWING
  // -------------------------------------
  function drawArena() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.beginPath();
    ctx.arc(arenaX, arenaY, arenaRadius, 0, Math.PI * 2);
    ctx.fillStyle = "#000";
    ctx.fill();
    ctx.lineWidth = 30;
    ctx.strokeStyle = "#fff";
    ctx.stroke();
  }

  function drawRobot() {
    ctx.save();
    ctx.translate(robot.x, robot.y);
    ctx.rotate(robot.angle);

    ctx.drawImage(
      robotImg,
      -robotSize / 2,
      -robotSize / 2,
      robotSize,
      robotSize
    );

    ctx.restore();
  }

  function drawBoxes() {
    boxes.forEach((b) => {
      ctx.drawImage(
        boxImg,
        b.x - boxSize / 2,
        b.y - boxSize / 2,
        boxSize,
        boxSize
      );
    });
  }

  // -------------------------------------
  // LOOP
  // -------------------------------------
  function loop() {
    updateRobot();
    drawArena();
    drawBoxes();
    drawRobot();
    requestAnimationFrame(loop);
  }

  // RESET BUTTON
  document.getElementById("reset-btn")!.addEventListener("click", resetSimulation);

  resetSimulation();
  loop();
}

export function initParticles(container: HTMLElement) {
    const particles = container.querySelectorAll(".particle");

    const loopAnimation = () => {
        animate(particles, {
            translateX: () => random(-200, 200),
            translateY: () => random(-200, 200),
            ease: "linear",
            duration: 8000,
            onComplete: () => loopAnimation(),
            delay: stagger(30, { grid: [10, 10], from: "center" }),
        })
    }

    loopAnimation();
}
