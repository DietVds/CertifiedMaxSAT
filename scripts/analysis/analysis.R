library(ggplot2)


# Read results
results <- read.csv(file = "/home/wolf/CertifiedMaxSAT/scripts/analysis/results2021.csv")
colnames(results) <- c("instance", "runtime", "totalizer", "mem", "runtime_w", "proofsize", "totalizer_w", "totalizer2_w", "mem_w", "runtime_v", "mem_v", "status")


#--------------------------------
# PLOTS
#--------------------------------

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
ggsave(plot = p, width = 8, height = 8, dpi = 300, filename = "./scripts/analysis/total.pdf")