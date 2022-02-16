library(ggplot2)
library(ggthemes)
theme_set(theme_light())

# Read results
results <- read.csv(file = "/home/wolf/CertifiedMaxSAT/scripts/analysis/results2021.csv")
colnames(results) <- c("instance", "runtime", "totalizer", "mem", "runtime_w", "proofsize", "totalizer_w", "totalizer2_w", "mem_w", "runtime_v", "mem_v", "status")


#--------------------------------
# PLOTS
#--------------------------------

# TYPE1
no_NAs <- results[!is.na(results$proofsize), ]
to <- 3600

ggplot(no_NAs, aes(x = runtime_w, y = runtime, color = log10((proofsize / 10^3) + 1))) +
    geom_point(shape = 4) +
    scale_x_log10(limits = c(0.1, 10000), breaks = c(1, 10, 100, 1000, 10000)) +
    scale_y_log10(limits = c(0.1, 10000), breaks = c(1, 10, 100, 1000, 10000)) +
    scale_color_continuous(breaks = c(3, 6, 9), labels = c("1MB", "1GB", "1TB")) +
    coord_fixed(ratio = 1) +
    geom_abline(slope = 1, intercept = 0, linetype = "dashed") +
    labs(color = "Proofsize") +
    xlab("QMaxSATpb (time in s)") +
    ylab("QMaxSAT (time in s)")

ggsave("./scripts/analysis/without_vs_with.pdf", device = "pdf", width = 20, height = 20, units = "cm")

# TYPE2
ggplot(no_NAs, aes(x = runtime_v, y = runtime_w)) +
    geom_point() +
    scale_x_log10(limits = c(0.1, 3 * to), breaks = c(1, 10, 100, 1000, 10000)) +
    scale_y_log10(limits = c(0.1, 10000), breaks = c(1, 10, 100, 1000, 10000)) +
    coord_fixed(ratio = 1) +
    geom_abline(slope = 1, intercept = 0, linetype = "dashed") +
    labs(color = "Requires Breaking", shape = "Requires Breaking") +
    theme(legend.position = "top") +
    xlab("VeriPB (time in s)") +
    ylab("QMaxSAT (time in s)")

ggsave("./scripts/analysis/solving_vs_verification.pdf", device = "pdf", width = 20, height = 20, units = "cm")

# Total solved instances
runtime <- sort(results[!is.na(results$runtime) & results$runtime < 3600, "runtime"])
runtime_w <- sort(results[!is.na(results$runtime_w) & results$runtime_w < 3600, "runtime_w"])
combined <- rbind(setNames(data.frame(runtime, cumsum(rep(1, length(runtime))), rep("QMaxSAT", length(runtime))), c("runtime", "solved_in_time", "solver")), setNames(data.frame(runtime_w, cumsum(rep(1, length(runtime_w))), rep("QMaxSATpb", length(runtime_w))), c("runtime", "solved_in_time", "solver")))

p <- ggplot(data = combined) +
    geom_line(aes(x = solved_in_time, y = runtime, colour = solver)) +
    geom_point(aes(x = solved_in_time, y = runtime, colour = solver), shape = 24) +
    labs(x = "Number of solved instances", y = "Time in seconds") +
    scale_colour_manual("Configuration", values = c("red", "blue")) +
    scale_y_continuous(sec.axis = dup_axis(name = NULL, labels = NULL)) +
    scale_x_continuous(sec.axis = dup_axis(name = NULL, labels = NULL)) +
    theme(
        legend.position = c(0.1, 0.9), panel.grid.major = element_blank(), panel.grid.minor = element_blank(),
        panel.background = element_blank(), axis.line = element_line(colour = "black"),
        axis.ticks.length = unit(-0.2, "cm")
    )
ggsave("./scripts/analysis/total.pdf", device = "pdf", width = 20, height = 20, units = "cm")