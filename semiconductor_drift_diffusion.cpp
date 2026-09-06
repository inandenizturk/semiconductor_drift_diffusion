cat << 'END_OF_CODE' > semiconductor_drift_diffusion.cpp
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <iomanip>

int main() {
    std::cout << "[+] Semiconductor Carrier Transport (12s video render) running...\n";

    // 12 saniye x 25 fps = 300 kare
    const int total_frames = 300;
    const int steps_per_frame = 8;
    const int total_steps = total_frames * steps_per_frame; // 2400 adim

    // +x yonunde elektrik alan (Drift) agirligi
    const double bias_prob = 0.37;
    const double normal_prob = 0.21;

    std::vector<int> x(total_steps + 1, 0);
    std::vector<int> y(total_steps + 1, 0);

    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};

    for (int i = 1; i <= total_steps; ++i) {
        double r = (double)std::rand() / RAND_MAX;
        int dir;
        if (r < bias_prob) dir = 0;
        else if (r < bias_prob + normal_prob) dir = 1;
        else if (r < bias_prob + 2 * normal_prob) dir = 2;
        else dir = 3;

        x[i] = x[i - 1] + dx[dir];
        y[i] = y[i - 1] + dy[dir];
    }

    std::ofstream tr_out("trajectory.dat");
    for (int i = 0; i <= total_steps; ++i) {
        tr_out << i << " " << x[i] << " " << y[i] << "\n";
    }
    tr_out.close();

    std::ofstream plt("render_walk.plt");
    plt << "set terminal pngcairo size 1200,600 background '#050508' font 'Helvetica,11'\n";
    plt << "set palette defined (0 '#050508', 0.2 '#002b5c', 0.45 '#00b4d8', 0.75 '#f77f00', 0.95 '#fcbf49', 1.0 '#ffffff')\n";

    plt << "do for [f=1:" << total_frames << "] {\n";
    plt << "  step = f * " << steps_per_frame << "\n";
    plt << "  set output sprintf('frame_%03d.png', f)\n";
    plt << "  system(sprintf('./get_pos trajectory.dat %d > current_step.info', step))\n";
    plt << "  set multiplot layout 1,2 title system('cat current_step.info') textcolor rgb '#ffffff'\n";

    // Sol Panel: Carrier Density (Heatmap)
    plt << "  set title 'Carrier Density Distribution n(x,y,t)' textcolor rgb '#cccccc'\n";
    plt << "  set xlabel 'x (Drift Axis)' textcolor rgb '#888888'\n";
    plt << "  set ylabel 'y (Transverse)' textcolor rgb '#888888'\n";
    plt << "  set border lc rgb '#444444'\n";
    plt << "  set tics textcolor rgb '#888888'\n";
    plt << "  set xrange [-20:180]\n";
    plt << "  set yrange [-60:60]\n";
    plt << "  set cbrange [0:1]\n";
    plt << "  unset colorbox\n";
    plt << "  set pm3d map\n";
    plt << "  splot 'current_pos.dat' using 1:2:3 notitle with pm3d\n";

    // Sag Panel: Lattice Trajectory
    plt << "  unset pm3d\n";
    plt << "  set title 'Electron Lattice Trajectory (E-Field Biased)' textcolor rgb '#cccccc'\n";
    plt << "  set xlabel 'x (Lattice units)' textcolor rgb '#888888'\n";
    plt << "  set ylabel 'y (Lattice units)' textcolor rgb '#888888'\n";
    plt << "  set xrange [-20:180]\n";
    plt << "  set yrange [-60:60]\n";
    plt << "  set grid lc rgb '#1a1a24' dt 1 lw 1\n";
    plt << "  plot 'trajectory.dat' every ::0::step using 2:3 with steps lw 1.5 lc rgb '#64dfdf' title '', \\\n";
    plt << "       'trajectory.dat' every ::0::0 using 2:3 with points pt 7 ps 1.8 lc rgb '#ff0055' title '', \\\n";
    plt << "       'trajectory.dat' every ::step::step using 2:3 with points pt 7 ps 2.0 lc rgb '#ffffff' title ''\n";

    plt << "  unset multiplot\n";
    plt << "}\n";
    plt.close();

    std::ofstream h_out("get_pos.cpp");
    h_out << "#include <iostream>\n#include <fstream>\n#include <cmath>\n#include <iomanip>\n";
    h_out << "int main(int argc, char* argv[]) {\n";
    h_out << "    int target = std::stoi(argv[2]);\n";
    h_out << "    std::ifstream in(argv[1]);\n";
    h_out << "    int s; double x, y, cx=0, cy=0;\n";
    h_out << "    while(in >> s >> x >> y) {\n";
    h_out << "        if(s == target) { cx = x; cy = y; break; }\n";
    h_out << "    }\n";
    h_out << "    std::cout << \"Semiconductor Electron Transport   step=\" << target "
          << "              << \"   drift x=\" << std::fixed << std::setprecision(1) << cx "
          << "              << \"   diffusion sigma~\" << std::fixed << std::setprecision(1) << std::sqrt(12.0 + target * 0.16) << \"\\n\";\n";
    h_out << "    std::ofstream pos(\"current_pos.dat\");\n";
    h_out << "    double sig = std::sqrt(12.0 + target * 0.20);\n";
    h_out << "    for(double gx=-20; gx<=180; gx+=3.0) {\n";
    h_out << "        for(double gy=-60; gy<=60; gy+=3.0) {\n";
    h_out << "            double r2 = (gx - cx)*(gx - cx) + (gy - cy)*(gy - cy);\n";
    h_out << "            double val = std::exp(-r2 / (2.0 * sig * sig));\n";
    h_out << "            pos << gx << \" \" << gy << \" \" << val << \"\\n\";\n";
    h_out << "        }\n";
    h_out << "        pos << \"\\n\";\n";
    h_out << "    }\n";
    h_out << "    return 0;\n";
    h_out << "}\n";
    h_out.close();

    system("clang++ -std=c++17 get_pos.cpp -o get_pos");

    std::cout << "[+] Rendering 300 frames with Gnuplot...\n";
    system("gnuplot render_walk.plt");

    std::cout << "[+] 12 second MP4 copy (ffmpeg)...\n";
    system("ffmpeg -y -r 25 -i frame_%03d.png -c:v libx264 -pix_fmt yuv420p semiconductor_drift_diffusion.mp4 2>/dev/null");
    system("rm -f frame_*.png render_walk.plt trajectory.dat get_pos.cpp get_pos current_pos.dat current_step.info");

    std::cout << "[+] semiconductor_drift_diffusion.mp4 is ready! Opening...\n";
    system("open semiconductor_drift_diffusion.mp4");

    return 0;
}
END_OF_CODE

clang++ -std=c++17 -O3 semiconductor_drift_diffusion.cpp -o semiconductor_drift_diffusion && ./semiconductor_drift_diffusion
